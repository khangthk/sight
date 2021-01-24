/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#define CAMP_COMPILATION

#include "arData/CameraSeries.hpp"
#include "arData/detail/reflection/CameraSeries.hpp"

#include <core/reflection/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((arData)(CameraSeries))
{
    builder
    .tag("object_version", "3")
    .tag("lib_name", "arData")
    .base< sight::data::Series>()
    .property("cameras", &::arData::CameraSeries::m_cameras)
    .property("extrinsic_matrices", &::arData::CameraSeries::m_extrinsicMatrices)
    ;
}
