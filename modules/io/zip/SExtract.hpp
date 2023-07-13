/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/io/zip/config.hpp"

#include <core/com/Signal.hpp>
#include <core/jobs/IJob.hpp>

#include <io/base/service/IReader.hpp>

namespace sight::module::io::zip
{

/**
 * @brief Archive extractor
 *
 * @details Service to extract a zip archive to a directory. The archive might be password protected using AES256; if
 * so, a popup will ask the user for the password.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(core::jobs::IJob)): emitted to display a progress bar while the image is written (it should be
 * connected to a SJobBar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::zip::SExtract" />
   @endcode
 *
 * @see sight::io::base::service::IReader
 * @see sight::io::zip::ArchiveReader
 */

class MODULE_IO_ZIP_CLASS_API SExtract final : public sight::io::base::service::IReader
{
public:

    SIGHT_DECLARE_SERVICE(SExtract, sight::io::base::service::IReader);

    using JobCreatedSignal = core::com::Signal<void (core::jobs::IJob::sptr)>;

    MODULE_IO_ZIP_API SExtract() noexcept;

    MODULE_IO_ZIP_API ~SExtract() noexcept override;

    /// Propose to read an archive
    MODULE_IO_ZIP_API void openLocationDialog() override;

protected:

    /// Does nothing
    MODULE_IO_ZIP_API void starting() override;

    /// Does nothing
    MODULE_IO_ZIP_API void stopping() override;

    /// Parses the configuration
    MODULE_IO_ZIP_API void configuring() override;

    /// Extract an archive to the specified directory
    MODULE_IO_ZIP_API void updating() override;

    /// Returns managed path type, here service manages only single file
    MODULE_IO_ZIP_API sight::io::base::service::IOPathType getIOPathType() const override
    {
        return sight::io::base::service::FILE;
    }

private:

    class ExtractImpl;
    std::unique_ptr<ExtractImpl> m_pimpl;
};

} // namespace sight::module::io::zip
