/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "grabber.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

namespace sight::io::service
{

const core::com::signals::key_t grabber::POSITION_MODIFIED_SIG = "positionModified";
const core::com::signals::key_t grabber::DURATION_MODIFIED_SIG = "durationModified";

const core::com::signals::key_t grabber::CAMERA_STARTED_SIG = "cameraStarted";
const core::com::signals::key_t grabber::CAMERA_STOPPED_SIG = "cameraStopped";

const core::com::signals::key_t grabber::FRAME_PRESENTED_SIG = "framePresented";

const core::com::signals::key_t grabber::PARAMETER_CHANGED_SIG = "parameterChanged";

const core::com::signals::key_t grabber::JOB_CREATED_SIG = "jobCreated";
const core::com::signals::key_t grabber::FPS_CHANGED_SIG = "fpsChanged";

const core::com::slots::key_t grabber::START_CAMERA_SLOT       = "startCamera";
const core::com::slots::key_t grabber::STOP_CAMERA_SLOT        = "stopCamera";
const core::com::slots::key_t grabber::PAUSE_CAMERA_SLOT       = "pauseCamera";
const core::com::slots::key_t grabber::PLAY_PAUSE_CAMERA_SLOT  = "playPauseCamera";
const core::com::slots::key_t grabber::LOOP_VIDEO_SLOT         = "loopVideo";
const core::com::slots::key_t grabber::SET_POSITION_VIDEO_SLOT = "setPositionVideo";
const core::com::slots::key_t grabber::PREVIOUS_IMAGE_SLOT     = "previousImage";
const core::com::slots::key_t grabber::NEXT_IMAGE_SLOT         = "nextImage";
const core::com::slots::key_t grabber::SET_STEP_SLOT           = "setStep";

const core::com::slots::key_t grabber::SET_PARAMETER_SLOT = "setParameter";

const core::com::slots::key_t grabber::REQUEST_SETTINGS_SLOT    = "requestSettings";
const core::com::slots::key_t grabber::OPTIMIZE_SLOT            = "optimize";
const core::com::slots::key_t grabber::FORWARD_FPS_CHANGED_SLOT = "forwardFPSChanged";

const std::string grabber::ADD_ROI_CENTER_SLOT    = "addROICenter";
const std::string grabber::REMOVE_ROI_CENTER_SLOT = "removeROICenter";

// ----------------------------------------------------------------------------

grabber::grabber() noexcept :
    notifier(m_signals)
{
    //Declare all signals
    new_signal<PositionModifiedSignalType>(POSITION_MODIFIED_SIG);
    new_signal<DurationModifiedSignalType>(DURATION_MODIFIED_SIG);
    new_signal<CameraStartedSignalType>(CAMERA_STARTED_SIG);
    new_signal<CameraStoppedSignalType>(CAMERA_STOPPED_SIG);
    new_signal<FramePresentedSignalType>(FRAME_PRESENTED_SIG);

    new_signal<ParameterChangedSignalType>(PARAMETER_CHANGED_SIG);
    new_signal<JobCreatedSignalType>(JOB_CREATED_SIG);
    new_signal<FPSChangedSignalType>(FPS_CHANGED_SIG);

    new_slot(START_CAMERA_SLOT, &grabber::startCamera, this);
    new_slot(STOP_CAMERA_SLOT, &grabber::stopCamera, this);
    new_slot(PAUSE_CAMERA_SLOT, &grabber::pauseCamera, this);
    new_slot(PLAY_PAUSE_CAMERA_SLOT, &grabber::playPauseCamera, this);
    new_slot(LOOP_VIDEO_SLOT, &grabber::toggleLoopMode, this);
    new_slot(SET_POSITION_VIDEO_SLOT, &grabber::setPosition, this);
    new_slot(PREVIOUS_IMAGE_SLOT, &grabber::previousImage, this);
    new_slot(NEXT_IMAGE_SLOT, &grabber::nextImage, this);
    new_slot(SET_STEP_SLOT, &grabber::setStep, this);

    new_slot(SET_PARAMETER_SLOT, &grabber::setParameter, this);

    new_slot(REQUEST_SETTINGS_SLOT, &grabber::requestSettings, this);
    new_slot(OPTIMIZE_SLOT, &grabber::optimize, this);

    new_slot(ADD_ROI_CENTER_SLOT, &grabber::addROICenter, this);
    new_slot(REMOVE_ROI_CENTER_SLOT, &grabber::removeROICenter, this);

    new_slot(FORWARD_FPS_CHANGED_SLOT, &grabber::forwardFPSChanged, this);
}

// ----------------------------------------------------------------------------

void grabber::playPauseCamera()
{
    if(m_isStarted)
    {
        this->pauseCamera();
    }
    else
    {
        this->startCamera();
    }
}

// ----------------------------------------------------------------------------
void grabber::previousImage()
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void grabber::nextImage()
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void grabber::setStep(int /*step*/, std::string /*key*/)
{
    SIGHT_WARN("Frame by frame mode not implemented for this grabber type.");
}

//-----------------------------------------------------------------------------

void grabber::setParameter(ui::parameter_t /*unused*/, std::string /*unused*/)
{
}

// ----------------------------------------------------------------------------

void grabber::requestSettings()
{
}

// ----------------------------------------------------------------------------

void grabber::optimize()
{
}

//------------------------------------------------------------------------------

void grabber::addROICenter(sight::data::Point::sptr /*unused*/)
{
}

//------------------------------------------------------------------------------

void grabber::removeROICenter(sight::data::Point::sptr /*unused*/)
{
}

//------------------------------------------------------------------------------

void grabber::clearTimeline(data::FrameTL& _tl)
{
    if(_tl.isAllocated())
    {
        // Clear the timeline: send a black frame
        const core::hires_clock::type timestamp = _tl.getNewerTimestamp() + 1;

        SPTR(data::FrameTL::BufferType) buffer = _tl.createBuffer(timestamp);
        auto* destBuffer = reinterpret_cast<std::uint8_t*>(buffer->addElement(0));

        std::memset(destBuffer, 0, _tl.getWidth() * _tl.getHeight() * _tl.numComponents());

        // push buffer and notify
        _tl.clearTimeline();
        _tl.pushObject(buffer);

        auto sigTL = _tl.signal<data::TimeLine::ObjectPushedSignalType>(
            data::TimeLine::OBJECT_PUSHED_SIG
        );
        sigTL->async_emit(timestamp);
    }
}

// ----------------------------------------------------------------------------

void grabber::setStartState(bool state)
{
    m_isStarted = state;
}

//------------------------------------------------------------------------------

void grabber::forwardFPSChanged(double /* fps */)
{
}

// ----------------------------------------------------------------------------

} // namespace sight::io::service
