/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

// cspell:ignore NOLINTNEXTLINE

#include "SFrameWriter.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/location/single_folder.hpp>

#include <data/Composite.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <filesystem>

namespace sight::module::io::video
{

static const core::com::slots::key_t SAVE_FRAME           = "saveFrame";
static const core::com::slots::key_t START_RECORD         = "startRecord";
static const core::com::slots::key_t STOP_RECORD          = "stopRecord";
static const core::com::slots::key_t RECORD               = "record";
static const core::com::slots::key_t TOGGLE_RECORDING     = "toggleRecording";
static const core::com::slots::key_t WRITE                = "write";
static const core::com::slots::key_t SET_FORMAT_PARAMETER = "setFormatParameter";

//------------------------------------------------------------------------------

SFrameWriter::SFrameWriter() noexcept :

    m_format(".tiff")
{
    new_slot(SAVE_FRAME, &SFrameWriter::saveFrame, this);
    new_slot(START_RECORD, &SFrameWriter::startRecord, this);
    new_slot(STOP_RECORD, &SFrameWriter::stopRecord, this);
    new_slot(RECORD, &SFrameWriter::record, this);
    new_slot(TOGGLE_RECORDING, &SFrameWriter::toggleRecording, this);
    new_slot(WRITE, &SFrameWriter::write, this);
    new_slot(SET_FORMAT_PARAMETER, &SFrameWriter::setFormatParameter, this);
}

//------------------------------------------------------------------------------

SFrameWriter::~SFrameWriter() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType SFrameWriter::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void SFrameWriter::configuring()
{
    sight::io::service::writer::configuring();

    service::config_t config = this->getConfiguration();

    m_format = config.get<std::string>("format", ".tiff");
}

//------------------------------------------------------------------------------

void SFrameWriter::starting()
{
}

//------------------------------------------------------------------------------

void SFrameWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();
    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the frames" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::stopping()
{
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::updating()
{
    core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();
    this->saveFrame(timestamp);
}

//------------------------------------------------------------------------------

void SFrameWriter::saveFrame(core::hires_clock::type _timestamp)
{
    this->startRecord();
    this->write(_timestamp);
    this->stopRecord();
}

//------------------------------------------------------------------------------

void SFrameWriter::write(core::hires_clock::type timestamp)
{
    if(m_isRecording)
    {
        // Retrieve dataStruct associated with this service
        const auto locked  = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::FrameTL>(locked.get_shared());

        // The following lock causes the service to drop frames if under heavy load. This prevents desynchronization
        // between frames and timestamps.
        // TODO: experiment with queuing frames and writing them from a worker thread.
        const auto sig = frameTL->signal<data::FrameTL::ObjectPushedSignalType>(
            data::FrameTL::OBJECT_PUSHED_SIG
        );
        core::com::connection::blocker writeBlocker(sig->get_connection(m_slots[WRITE]));

        // Get the buffer of the copied timeline
        const auto buffer = frameTL->getClosestBuffer(timestamp);

        if(buffer)
        {
            timestamp = buffer->getTimestamp();
            const int width  = static_cast<int>(frameTL->getWidth());
            const int height = static_cast<int>(frameTL->getHeight());

            const std::uint8_t* imageBuffer = &buffer->getElement(0);

            cv::Mat image(cv::Size(width, height), m_imageType, (void*) imageBuffer, cv::Mat::AUTO_STEP);

            const auto time = static_cast<std::size_t>(timestamp);
            const std::string filename("img_" + std::to_string(time) + m_format);
            const std::filesystem::path path = this->get_folder() / filename;

            if(image.type() == CV_8UC3)
            {
                // convert the read image from BGR to RGB
                cv::Mat imageRgb;
                cv::cvtColor(image, imageRgb, cv::COLOR_BGR2RGB);
                cv::imwrite(path.string(), imageRgb);
            }
            else if(image.type() == CV_8UC4)
            {
                // convert the read image from BGRA to RGBA
                cv::Mat imageRgb;
                cv::cvtColor(image, imageRgb, cv::COLOR_BGRA2RGBA);
                cv::imwrite(path.string(), imageRgb);
            }
            else
            {
                cv::imwrite(path.string(), image);
            }
        }
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::startRecord()
{
    if(!this->hasLocationDefined())
    {
        this->openLocationDialog();
    }

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked  = m_data.lock();
        const auto frameTL = std::dynamic_pointer_cast<const data::FrameTL>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::FrameTL::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            frameTL
        );

        if(frameTL->getType() == core::type::UINT8 && frameTL->numComponents() == 3)
        {
            m_imageType = CV_8UC3;
        }
        else if(frameTL->getType() == core::type::UINT8 && frameTL->numComponents() == 4)
        {
            m_imageType = CV_8UC4;
        }
        else if(frameTL->getType() == core::type::UINT8 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_8UC1;
        }
        else if(frameTL->getType() == core::type::UINT16 && frameTL->numComponents() == 1)
        {
            m_imageType = CV_16UC1;
        }
        else
        {
            SIGHT_ERROR(
                "This type of frame : " + frameTL->getType().name() + " with "
                + std::to_string(frameTL->numComponents()) + " is not supported"
            );
            return;
        }

        std::filesystem::path path = this->get_folder();

        if(!std::filesystem::exists(path))
        {
            std::filesystem::create_directories(path);
        }

        m_isRecording = true;
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::stopRecord()
{
    m_isRecording = false;
}

//------------------------------------------------------------------------------

void SFrameWriter::toggleRecording()
{
    if(m_isRecording)
    {
        this->stopRecord();
    }
    else
    {
        this->startRecord();
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::record(bool state)
{
    if(state)
    {
        this->startRecord();
    }
    else
    {
        this->stopRecord();
    }
}

//------------------------------------------------------------------------------

void SFrameWriter::setFormatParameter(std::string val, std::string key)
{
    if(key == "format")
    {
        if(val == ".tiff"
           || val == ".jpeg"
           || val == ".bmp"
           || val == ".png"
           || val == ".jp2")
        {
            m_format = val;
        }
        else
        {
            SIGHT_ERROR("Value : '" + val + "' is not supported");
        }
    }
    else
    {
        SIGHT_ERROR("The slot key : '" + key + "' is not handled");
    }
}

//------------------------------------------------------------------------------

service::connections_t SFrameWriter::getAutoConnections() const
{
    service::connections_t connections;
    connections.push(sight::io::service::s_DATA_KEY, data::FrameTL::OBJECT_PUSHED_SIG, WRITE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
