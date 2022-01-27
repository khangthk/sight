/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/io/itk/config.hpp"

#include <data/ImageSeries.hpp>

#include <io/base/service/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::core::jobs
{

class IJob;

}

namespace sight::module::io::itk
{

/**
 * @brief   ITK ImageSeries Reader.
 *
 * Service reading an image series using the io_itk lib.
 *
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::io::itk::SImageSeriesReader">
        <inout key="data" uid="imageSeries" />
        <file>@path/to/file</file>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [sight::data::ImageSeries]: ImageSeries containing read image
 *
 */
class MODULE_IO_ITK_CLASS_API SImageSeriesReader : public sight::io::base::service::IReader
{
public:

    typedef core::com::Signal<void (SPTR(core::jobs::IJob))> JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_IO_ITK_API SImageSeriesReader() noexcept;

    ~SImageSeriesReader() noexcept override
    {
    }

    SIGHT_DECLARE_SERVICE(SImageSeriesReader, sight::io::base::service::IReader);

    /**
     * @brief Configure the image path.
     */
    MODULE_IO_ITK_API void openLocationDialog() override;

protected:

    MODULE_IO_ITK_API sight::io::base::service::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    MODULE_IO_ITK_API void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    MODULE_IO_ITK_API void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    MODULE_IO_ITK_API void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    MODULE_IO_ITK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give informations about the service.
     */
    MODULE_IO_ITK_API void info(std::ostream& _sstream) override;

private:

    /// Image path.
    std::filesystem::path m_fsImgPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace sight::module::io::itk