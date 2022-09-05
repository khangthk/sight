/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "io/vtk/config.hpp"

#include <core/location/SingleFile.hpp>

#include <data/Mesh.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class Observer;
class IJob;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief Reads a mesh from a .vtp file.
 *
 * Reads a VTK Mesh (.vtp) using the VTK library.
 */

class IO_VTK_CLASS_API VtpMeshReader :
    public base::reader::GenericObjectReader<data::Mesh>,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        VtpMeshReader,
        io::base::reader::GenericObjectReader<data::Mesh>,
        io::base::reader::factory::New<VtpMeshReader>
    )
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// @brief Constructor.
    IO_VTK_API VtpMeshReader(io::base::reader::IObjectReader::Key key);

    /// @brief Destructor.
    IO_VTK_API ~VtpMeshReader() override;

    /// @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return ".vtp"
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;
};

} // namespace sight::io::vtk
