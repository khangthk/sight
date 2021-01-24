/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "fwStructuralPatch/creator/data/PointList1.hpp"

#include <fwAtoms/Sequence.hpp>

#include <fwAtomsPatch/helper/Object.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

// ----------------------------------------------------------------------------

PointList1::PointList1()
{
    m_classname = "data::PointList";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

PointList1::~PointList1()
{
}

// ----------------------------------------------------------------------------

PointList1::PointList1( const PointList1& cpy ) :
    ::fwAtomsPatch::IStructuralCreator(cpy)
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr PointList1::create()
{
    ::fwAtoms::Object::sptr pl = this->createObjBase();
    ::fwAtomsPatch::helper::Object helper(pl);

    helper.addAttribute("points", ::fwAtoms::Sequence::New());

    return pl;
}

// ----------------------------------------------------------------------------

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
