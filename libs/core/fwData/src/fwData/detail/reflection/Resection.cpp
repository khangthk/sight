/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#define CAMP_COMPILATION

#include "fwData/detail/reflection/Resection.hpp"

#include <core/reflection/UserObject.hpp>

fwCampImplementDataMacro((fwData)(Resection))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwData")
    .base< ::fwData::Object>()
    .property("name", &::fwData::Resection::m_name)
    .property("plane_list", &::fwData::Resection::m_planeList)
    .property("inputs", &::fwData::Resection::m_vInputs)
    .property("outputs", &::fwData::Resection::m_vOutputs)
    .property("is_safe_part", &::fwData::Resection::m_isSafePart)
    .property("is_valid", &::fwData::Resection::m_isValid)
    .property("is_visible", &::fwData::Resection::m_isVisible)
    ;
}
