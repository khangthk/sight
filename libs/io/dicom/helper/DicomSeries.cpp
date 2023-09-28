/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/dicom/helper/DicomSeries.hpp"

#include "io/dicom/helper/DicomDir.hpp"

#include <core/exceptionmacros.hpp>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>
#include <core/spy_log.hpp>

#include <data/DicomSeries.hpp>

#include <boost/algorithm/string.hpp>

#include <gdcmMediaStorage.h>
#include <gdcmReader.h>

#include <algorithm>
#include <filesystem>

namespace sight::io::dicom::helper
{

// Series
static const gdcm::Tag s_MediaStorageSOPClassUID(0x0002, 0x0002);
static const gdcm::Tag s_SpecificCharacterSetTag(0x0008, 0x0005);
static const gdcm::Tag s_SeriesInstanceUIDTag(0x0020, 0x000e);
static const gdcm::Tag s_SeriesDateTag(0x0008, 0x0021);
static const gdcm::Tag s_SeriesTimeTag(0x0008, 0x0031);
static const gdcm::Tag s_ModalityTag(0x0008, 0x0060);
static const gdcm::Tag s_SeriesDescriptionTag(0x0008, 0x103e);
static const gdcm::Tag s_PerformingPhysicianNameTag(0x0008, 0x1050);
static const gdcm::Tag s_SOPClassUIDTag(0x0008, 0x0016);
static const gdcm::Tag s_SOPInstanceUIDTag(0x0008, 0x0018);

// Equipment
static const gdcm::Tag s_InstitutionNameTag(0x0008, 0x0080);

// Patient
static const gdcm::Tag s_PatientNameTag(0x0010, 0x0010);
static const gdcm::Tag s_PatientIDTag(0x0010, 0x0020);
static const gdcm::Tag s_PatientBirthDateTag(0x0010, 0x0030);
static const gdcm::Tag s_PatientSexTag(0x0010, 0x0040);

// Study
static const gdcm::Tag s_StudyInstanceUIDTag(0x0020, 0x000d);
static const gdcm::Tag s_StudyDateTag(0x0008, 0x0020);
static const gdcm::Tag s_StudyTimeTag(0x0008, 0x0030);
static const gdcm::Tag s_ReferringPhysicianNameTag(0x0008, 0x0090);
static const gdcm::Tag s_StudyDescriptionTag(0x0008, 0x1030);
static const gdcm::Tag s_PatientAgeTag(0x0010, 0x1010);

//------------------------------------------------------------------------------

std::string getStringValue(
    const gdcm::Scanner& scanner,
    const std::string& filename,
    const gdcm::Tag& tag
)
{
    std::string result;
    const char* value = scanner.GetValue(filename.c_str(), tag);
    if(value != nullptr)
    {
        // Trim buffer
        result = gdcm::LOComp::Trim(value);
    }

    return result;
}

//------------------------------------------------------------------------------

std::string getStringValue(
    const gdcm::DataSet& dataset,
    const gdcm::Tag& tag
)
{
    std::string result;
    if(dataset.FindDataElement(tag))
    {
        const gdcm::DataElement& dataElement = dataset.GetDataElement(tag);

        if(!dataElement.IsEmpty())
        {
            // Retrieve buffer
            const gdcm::ByteValue* bv = dataElement.GetByteValue();
            if(bv != nullptr)
            {
                std::string buffer(bv->GetPointer(), bv->GetLength());

                // Trim buffer
                result = gdcm::LOComp::Trim(buffer.c_str());
            }
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

DicomSeries::DicomSeriesContainerType DicomSeries::read(
    FilenameContainerType& filenames,
    const SPTR(core::jobs::observer)& readerObserver,
    const SPTR(core::jobs::observer)& completeSeriesObserver
)
{
    DicomSeriesContainerType seriesContainer = DicomSeries::splitFiles(filenames, readerObserver);
    DicomSeries::fillSeries(seriesContainer, completeSeriesObserver);
    return seriesContainer;
}

//------------------------------------------------------------------------------

void DicomSeries::complete(
    DicomSeriesContainerType& seriesContainer,
    const SPTR(core::jobs::observer)& completeSeriesObserver
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(s_SpecificCharacterSetTag);
    selectedtags.insert(s_SeriesInstanceUIDTag);
    selectedtags.insert(s_ModalityTag);
    selectedtags.insert(s_SeriesDateTag);
    selectedtags.insert(s_SeriesTimeTag);
    selectedtags.insert(s_SeriesDescriptionTag);
    selectedtags.insert(s_PerformingPhysicianNameTag);
    selectedtags.insert(s_SOPClassUIDTag);
    selectedtags.insert(s_SOPInstanceUIDTag);

    for(const auto& series : seriesContainer)
    {
        if(series->getDicomContainer().empty())
        {
            SIGHT_ERROR("DicomSeries doesn't not contain any instance.");
            break;
        }

        const auto& firstItem                                      = series->getDicomContainer().begin();
        const core::memory::buffer_object::sptr bufferObj          = firstItem->second;
        const core::memory::buffer_manager::stream_info streamInfo = bufferObj->get_stream_info();
        SPTR(std::istream) is = streamInfo.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << firstItem->first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        //Modality
        std::string modality = getStringValue(dataset, s_ModalityTag);
        series->setModality(modality);

        //Date
        std::string seriesDate = getStringValue(dataset, s_SeriesDateTag);
        series->setSeriesDate(seriesDate);

        //Time
        std::string seriesTime = getStringValue(dataset, s_SeriesTimeTag);
        series->setSeriesTime(seriesTime);

        //Description
        std::string seriesDescription = getStringValue(dataset, s_SeriesDescriptionTag);
        series->setSeriesDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianName = getStringValue(dataset, s_PerformingPhysicianNameTag);
        series->setPerformingPhysicianName(performingPhysicianName);

        // Add the SOPClassUID to the series
        const std::string& sopClassUID = getStringValue(dataset, s_SOPClassUIDTag);
        series->getSOPClassUIDs().insert(sopClassUID);
    }

    DicomSeries::fillSeries(seriesContainer, completeSeriesObserver);
}

//------------------------------------------------------------------------------

DicomSeries::DicomSeriesContainerType DicomSeries::splitFiles(
    FilenameContainerType& filenames,
    const core::jobs::observer::sptr& readerObserver
)
{
    gdcm::Scanner seriesScanner;
    seriesScanner.AddTag(s_SpecificCharacterSetTag);
    seriesScanner.AddTag(s_SeriesInstanceUIDTag);
    seriesScanner.AddTag(s_ModalityTag);
    seriesScanner.AddTag(s_SeriesDateTag);
    seriesScanner.AddTag(s_SeriesTimeTag);
    seriesScanner.AddTag(s_SeriesDescriptionTag);
    seriesScanner.AddTag(s_PerformingPhysicianNameTag);
    seriesScanner.AddTag(s_SOPClassUIDTag);
    seriesScanner.AddTag(s_SOPInstanceUIDTag);
    seriesScanner.AddTag(s_MediaStorageSOPClassUID);
    readerObserver->set_total_work_units(filenames.size());
    readerObserver->done_work(0);

    std::vector<std::string> fileVec;
    for(const auto& file : filenames)
    {
        fileVec.push_back(file.string());
    }

    bool status = seriesScanner.Scan(fileVec);
    SIGHT_THROW_IF("Unable to read the files.", !status);

    gdcm::Directory::FilenamesType keys = seriesScanner.GetKeys();

    unsigned int progress = 0;

    DicomSeriesContainerType seriesContainer;

    //Loop through every files available in the scanner
    std::map<std::string, std::filesystem::path> orderedFilenames;
    for(const std::filesystem::path& dicomFile : filenames)
    {
        orderedFilenames[dicomFile.filename().string()] = dicomFile;
    }

    std::set<std::string> previousSOPInstanceUIDs;

    for(const auto& dicomFile : orderedFilenames)
    {
        auto filename = dicomFile.second.string();

        SIGHT_ASSERT(
            "The file \"" << dicomFile.second << "\" is not a key of the gdcm scanner",
            seriesScanner.IsKey(filename.c_str())
        );

        const std::string& sopInstanceUID = getStringValue(seriesScanner, filename, s_SOPInstanceUIDTag);

        if(previousSOPInstanceUIDs.find(sopInstanceUID) != previousSOPInstanceUIDs.end())
        {
            SIGHT_WARN(
                "The instance "
                << sopInstanceUID
                << " has already been read, which usually means DICOM files are corrupted."
            );

            continue;
        }

        const std::string& sopClassUID             = getStringValue(seriesScanner, filename, s_SOPClassUIDTag);
        const std::string& mediaStorageSopClassUID = getStringValue(seriesScanner, filename, s_MediaStorageSOPClassUID);

        if(sopClassUID != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage)
           && mediaStorageSopClassUID
           != gdcm::MediaStorage::GetMSString(gdcm::MediaStorage::MediaStorageDirectoryStorage))
        {
            sight::io::dicom::helper::DicomSeries::createSeries(seriesContainer, seriesScanner, dicomFile.second);
            previousSOPInstanceUIDs.insert(sopInstanceUID);
        }

        if(!readerObserver || readerObserver->cancel_requested())
        {
            break;
        }

        readerObserver->done_work(static_cast<std::uint64_t>(++progress * 100LL / keys.size()));
    }

    return seriesContainer;
}

//------------------------------------------------------------------------------

void DicomSeries::fillSeries(
    DicomSeriesContainerType& seriesContainer,
    const core::jobs::observer::sptr& completeSeriesObserver
)
{
    std::set<gdcm::Tag> selectedtags;
    selectedtags.insert(s_SpecificCharacterSetTag);
    selectedtags.insert(s_PatientIDTag);
    selectedtags.insert(s_PatientNameTag);
    selectedtags.insert(s_PatientBirthDateTag);
    selectedtags.insert(s_PatientSexTag);
    selectedtags.insert(s_StudyInstanceUIDTag);
    selectedtags.insert(s_StudyDateTag);
    selectedtags.insert(s_StudyTimeTag);
    selectedtags.insert(s_ReferringPhysicianNameTag);
    selectedtags.insert(s_StudyDescriptionTag);
    selectedtags.insert(s_PatientAgeTag);
    selectedtags.insert(s_InstitutionNameTag);
    selectedtags.insert(s_SeriesInstanceUIDTag);

    std::uint64_t progress = 0;

    // Fill series
    for(const data::DicomSeries::sptr& series : seriesContainer)
    {
        // Compute number of instances
        const std::size_t size = series->getDicomContainer().size();
        series->setNumberOfInstances(size);

        if(size == 0U)
        {
            SIGHT_ERROR("The DicomSeries doesn't contain any instance.");
            break;
        }

        // Load first instance
        const auto& firstItem                                      = series->getDicomContainer().begin();
        const core::memory::buffer_object::sptr bufferObj          = firstItem->second;
        const core::memory::buffer_manager::stream_info streamInfo = bufferObj->get_stream_info();
        SPTR(std::istream) is = streamInfo.stream;

        gdcm::Reader reader;
        reader.SetStream(*is);

        if(!reader.ReadSelectedTags(selectedtags))
        {
            SIGHT_THROW(
                "Unable to read Dicom file '" << bufferObj->get_stream_info().fs_file.string() << "' "
                << "(slice: '" << firstItem->first << "')"
            );
        }

        const gdcm::DataSet& dataset = reader.GetFile().GetDataSet();

        // Fill series
        // Equipment Module
        const auto& institutionName = getStringValue(dataset, s_InstitutionNameTag);
        series->setInstitutionName(institutionName);

        // Patient Module
        const auto& patientID = getStringValue(dataset, s_PatientIDTag);
        series->setPatientID(patientID);

        const auto& patientName = getStringValue(dataset, s_PatientNameTag);
        series->setPatientName(patientName);

        const auto& patientBirthDate = getStringValue(dataset, s_PatientBirthDateTag);
        series->setPatientBirthDate(patientBirthDate);

        const auto& patientSex = getStringValue(dataset, s_PatientSexTag);
        series->setPatientSex(patientSex);

        // Study Module
        const auto& studyInstanceUID = getStringValue(dataset, s_StudyInstanceUIDTag);
        series->setStudyInstanceUID(studyInstanceUID);

        //Study Date
        const auto& studyDate = getStringValue(dataset, s_StudyDateTag);
        series->setStudyDate(studyDate);

        //Study Time
        const auto& studyTime = getStringValue(dataset, s_StudyTimeTag);
        series->setStudyTime(studyTime);

        //Referring Physician Name
        const auto& referringPhysicianName = getStringValue(dataset, s_ReferringPhysicianNameTag);
        series->setReferringPhysicianName(referringPhysicianName);

        //Study Description
        const auto& studyDescription = getStringValue(dataset, s_StudyDescriptionTag);
        series->setStudyDescription(studyDescription);

        //Study Patient Age
        const auto& patientAge = getStringValue(dataset, s_PatientAgeTag);
        series->setPatientAge(patientAge);

        if(completeSeriesObserver)
        {
            completeSeriesObserver->done_work(static_cast<std::uint64_t>(++progress * 100 / seriesContainer.size()));

            if(completeSeriesObserver->cancel_requested())
            {
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void DicomSeries::createSeries(
    DicomSeriesContainerType& seriesContainer,
    const gdcm::Scanner& scanner,
    const std::filesystem::path& filename
)
{
    data::DicomSeries::sptr series = data::DicomSeries::sptr();

    const std::string stringFilename = filename.string();

    // Get Series Instance UID
    std::string seriesInstanceUID = getStringValue(scanner, stringFilename, s_SeriesInstanceUIDTag);

    // Check if the series already exists
    for(const auto& dicomSeries : seriesContainer)
    {
        if(dicomSeries->getSeriesInstanceUID() == seriesInstanceUID)
        {
            series = dicomSeries;
            break;
        }
    }

    // If the series doesn't exist we create it
    if(!series)
    {
        series = std::make_shared<data::DicomSeries>();

        seriesContainer.push_back(series);

        //Instance UID
        series->setSeriesInstanceUID(seriesInstanceUID);

        //Modality
        std::string modality = getStringValue(scanner, stringFilename, s_ModalityTag);
        series->setModality(modality);

        //Date
        std::string seriesDate = getStringValue(scanner, stringFilename, s_SeriesDateTag);
        series->setSeriesDate(seriesDate);

        //Time
        std::string seriesTime = getStringValue(scanner, stringFilename, s_SeriesTimeTag);
        series->setSeriesTime(seriesTime);

        //Description
        std::string seriesDescription = getStringValue(scanner, stringFilename, s_SeriesDescriptionTag);
        series->setSeriesDescription(seriesDescription);

        //Performing Physicians Name
        std::string performingPhysicianName = getStringValue(scanner, stringFilename, s_PerformingPhysicianNameTag);
        series->setPerformingPhysicianName(performingPhysicianName);
    }

    // Add the SOPClassUID to the series
    const std::string& sopClassUID = getStringValue(
        scanner,
        stringFilename,
        s_SOPClassUIDTag
    );

    series->getSOPClassUIDs().insert(sopClassUID);
    series->addDicomPath(series->getDicomContainer().size(), filename);
}

} // namespace sight::io::dicom::helper
