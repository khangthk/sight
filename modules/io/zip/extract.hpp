/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/jobs/base.hpp>

#include <io/__/service/reader.hpp>

namespace sight::module::io::zip
{

/**
 * @brief Archive extractor
 *
 * @details Service to extract a zip archive to a directory. The archive might be password protected using AES256; if
 * so, a popup will ask the user for the password.
 *
 * @section Signals Signals
 * - \b job_created(SPTR(core::jobs::base)): emitted to display a progress bar while the image is written (it should be
 * connected to a job_bar).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::zip::extract" />
   @endcode
 *
 * @see sight::io::service::reader
 * @see sight::io::zip::archive_reader
 */

class extract final : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(extract, sight::io::service::reader);

    using job_created_signal_t = core::com::signal<void (core::jobs::base::sptr)>;

    extract() noexcept;

    ~extract() noexcept override;

    /// Propose to read an archive
    void open_location_dialog() override;

protected:

    /// Does nothing
    void starting() override;

    /// Does nothing
    void stopping() override;

    /// Parses the configuration
    void configuring() override;

    /// Extract an archive to the specified directory
    void updating() override;

    /// Returns managed path type, here service manages only single file
    sight::io::service::path_type_t get_path_type() const override
    {
        return sight::io::service::file;
    }

private:

    class extract_impl;
    std::unique_ptr<extract_impl> m_pimpl;
};

} // namespace sight::module::io::zip
