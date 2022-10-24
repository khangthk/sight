/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SSeriesSetWriter.hpp"

#include <core/base.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/Vector.hpp>

#include <io/base/service/IWriter.hpp>
#include <io/dicom/helper/Fiducial.hpp>
#include <io/dicom/writer/SeriesSet.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

namespace sight::module::io::dicom
{

//------------------------------------------------------------------------------

SSeriesSetWriter::SSeriesSetWriter() noexcept :
    m_fiducialsExportMode(sight::io::dicom::writer::Series::COMPREHENSIVE_3D_SR)
{
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result && this->selectFiducialsExportMode())
    {
        defaultDirectory->setFolder(result->getFolder());
        this->setFolder(result->getFolder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        const std::filesystem::path& folder = this->getFolder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::base::dialog::MessageDialog dialog;
            dialog.setMessage(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(ui::base::dialog::MessageDialog::QUESTION);
            dialog.addButton(sight::ui::base::dialog::MessageDialog::YES_NO);
            sight::ui::base::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == sight::ui::base::dialog::MessageDialog::NO)
            {
                return;
            }
        }

        // Retrieve dataStruct associated with this service
        const auto data   = m_data.lock();
        const auto vector = std::dynamic_pointer_cast<const data::Vector>(data.get_shared());

        // Create SeriesSet
        const auto series_set = data::SeriesSet::New();

        for(const auto& object : *vector)
        {
            const auto& series = data::Series::dynamicCast(object);
            SIGHT_ASSERT("The container should only contain series.", series);
            series_set->push_back(series);
        }

        sight::ui::base::BusyCursor cursor;

        saveSeriesSet(folder, series_set);
    }
}

//------------------------------------------------------------------------------

void SSeriesSetWriter::saveSeriesSet(const std::filesystem::path folder, data::SeriesSet::sptr series_set)
{
    m_writeFailed = true;

    auto writer = sight::io::dicom::writer::SeriesSet::New();
    writer->setObject(series_set);
    writer->setFiducialsExportMode(m_fiducialsExportMode);
    writer->setFolder(folder);

    try
    {
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving series ");
        writer->addHandler(progressMeterGUI);
        writer->write();

        m_writeFailed = false;
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::base::service::IOPathType SSeriesSetWriter::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

bool SSeriesSetWriter::selectFiducialsExportMode()
{
    // Retrieve dataStruct associated with this service
    const auto data   = m_data.lock();
    const auto vector = std::dynamic_pointer_cast<const data::Vector>(data.get_shared());

    // Create SeriesSet
    auto series_set = data::SeriesSet::New();

    for(const auto& object : *vector)
    {
        const auto& series = data::Series::dynamicCast(object);
        SIGHT_ASSERT("The container should only contain series.", series);
        series_set->push_back(series);
    }

    const bool containsLandmarks   = sight::io::dicom::helper::Fiducial::containsLandmarks(series_set);
    const bool containsDistances   = sight::io::dicom::helper::Fiducial::containsDistances(series_set);
    const bool contains3DDistances = sight::io::dicom::helper::Fiducial::contains3DDistances(series_set);

    if(containsLandmarks || containsDistances)
    {
        static const std::string fiducialIOD          = "Spatial Fiducials";
        static const std::string comprehensiveSRIOD   = "Comprehensive SR";
        static const std::string comprehensive3DSRIOD = "Comprehensive 3D SR";

        std::vector<std::string> exportModes;
        if(!containsDistances)
        {
            exportModes.push_back(fiducialIOD);
        }

        if(!contains3DDistances)
        {
            exportModes.push_back(comprehensiveSRIOD);
        }

        exportModes.push_back(comprehensive3DSRIOD);

        // Create selector
        auto selector = sight::ui::base::dialog::SelectorDialog::New();

        selector->setTitle("Fiducials export mode");
        selector->setSelections(exportModes);
        const std::string mode             = selector->show();
        const bool modeSelectionIsCanceled = mode.empty();

        if(mode == fiducialIOD)
        {
            m_fiducialsExportMode = sight::io::dicom::writer::Series::SPATIAL_FIDUCIALS;
        }
        else if(mode == comprehensiveSRIOD)
        {
            m_fiducialsExportMode = sight::io::dicom::writer::Series::COMPREHENSIVE_SR;
        }
        else
        {
            m_fiducialsExportMode = sight::io::dicom::writer::Series::COMPREHENSIVE_3D_SR;
        }

        if(mode.empty())
        {
            m_writeFailed = true;
        }

        return !modeSelectionIsCanceled;
    }

    return true;
}

} // namespace sight::module::io::dicom