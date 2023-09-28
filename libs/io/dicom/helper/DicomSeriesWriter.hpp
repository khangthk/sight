/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "io/dicom/config.hpp"

#include <core/location/single_folder.hpp>

#include <io/__/writer/GenericObjectWriter.hpp>

#include <string>

namespace sight::core::jobs
{

class observer;
class base;

} // namespace sight::core::jobs

namespace sight::data
{

class DicomSeries;

} // namespace sight::data

namespace sight::io::zip
{

class write_archive;

} // namespace sight::io::zip

namespace sight::io::dicom::helper
{

class DicomAnonymizer;

/**
 * @brief Write a DicomSeries in DICOM format.
 *
 * This writer optionally manages anonymization if an instance of DicomAnonymizer is set.
 * The output DICOM files can be exported into an archive (folder or zipped file) if output archive is set.
 *
 * If the DicomSeries contains the binaries of the DICOM files, the files are extracted (or zipped).
 * If the DicomSeries contains the paths of the DICOM files, a simple copy (or zip) is performed.
 */
class IO_DICOM_CLASS_API DicomSeriesWriter : public io::writer::GenericObjectWriter<data::DicomSeries>,
                                             public core::location::single_folder
{
public:

    SIGHT_DECLARE_CLASS(
        DicomSeriesWriter,
        io::writer::GenericObjectWriter<data::DicomSeries>,
        io::writer::factory::make<DicomSeriesWriter>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    /// Constructor. Does nothing
    IO_DICOM_API DicomSeriesWriter();

    /// Write the image series in DICOM format.
    IO_DICOM_API void write() override;

    /// Return an empty string
    IO_DICOM_API std::string extension() const override;

    /// Get job observer
    IO_DICOM_API SPTR(core::jobs::base) getJob() const override;

    /// Defines optional anonymizer.
    IO_DICOM_API void setAnonymizer(const SPTR(helper::DicomAnonymizer)& anonymizer);

    /**
     * @brief Defines output archive for DICOM files.
     *
     * @param archive Archive instance
     * @param subPath Path within archive where DICOM files are saved (only if an anonymizer instance is defined)
     */
    IO_DICOM_API void setOutputArchive(
        const SPTR(io::zip::write_archive)& archive,
        const std::string& subPath = ""
    );

protected:

    /// Compute DICOM filename according to anonymizer or return default filename.
    std::string getFilename(const std::size_t& instanceIndex);

    /// Process inputStream to outputStream with anonymization management.
    void processStream(std::istream& inputStream, std::ostream& outputStream);

    /// Process write on archive
    void processWriteArchive();

    /// Process write with Standard output stream
    void processWrite();

    /// Write count
    int m_writeCount {0};

    /// Job observer
    SPTR(core::jobs::observer) m_job;

    /// Optional anonymiser
    SPTR(DicomAnonymizer) m_anonymizer;

    /// Optional output archive
    SPTR(io::zip::write_archive) m_archive;

    /// Optional subPath (related to write archive
    std::string m_subPath;
};

} // namespace sight::io::dicom::helper
