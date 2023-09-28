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

#include "io/dicom/reader/iod/InformationObjectDefinition.hpp"

#include <core/log/logger.hpp>

namespace sight::io::dicom::reader
{

/**
 * @brief Series class used to read Series
 */
class IO_DICOM_CLASS_API Series
{
public:

    typedef std::map<SPTR(io::dicom::container::DicomInstance), data::Series::sptr> SeriesContainerMapType;

    typedef std::function<void (std::uint64_t)> ProgressCallback;
    typedef std::function<bool ()> CancelRequestedCallback;

    /// Constructor
    IO_DICOM_API Series();

    /// Destructor
    IO_DICOM_API ~Series();

    /**
     * @brief Read DICOM series
     * @param[in] dicomSeries DICOM series that shall be read
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API data::Series::sptr read(const data::DicomSeries::csptr& dicomSeries);

    /// Get Logger
    [[nodiscard]] const core::log::logger::sptr& getLogger() const
    {
        return m_logger;
    }

    /// Set Logger
    void setLogger(const core::log::logger::sptr& logger)
    {
        m_logger = logger;
    }

    /**
     * @brief Set progress callback
     * @param[in] callback Progress callback
     */
    void setProgressCallback(ProgressCallback callback)
    {
        m_progressCallback = callback;
    }

    /**
     * @brief Set cancel callback
     * @param[in] callback Cancel callback
     */
    void setCancelRequestedCallback(CancelRequestedCallback callback)
    {
        m_cancelRequestedCallback = callback;
    }

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

protected:

    /// Get referenced series when dealing with Spatial Fiducials
    SPTR(io::dicom::container::DicomInstance) getSpatialFiducialsReferencedSeriesInstance(
        const data::DicomSeries::csptr& dicomSeries
    );

    /// Get referenced series when dealing with Structured Report
    SPTR(io::dicom::container::DicomInstance) getStructuredReportReferencedSeriesInstance(
        const data::DicomSeries::csptr& dicomSeries
    );

    /// Series Container Map
    SeriesContainerMapType m_seriesContainerMap;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;

    /// Enable buffer rotation
    bool m_enableBufferRotation {true};
};

} // namespace sight::io::dicom::reader
