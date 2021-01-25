/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwStructuralPatch/creator/data/ModelSeries1.hpp"

#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <fwAtomsPatch/helper/Object.hpp>
#include <fwAtomsPatch/StructuralCreatorDB.hpp>

namespace fwStructuralPatch
{
namespace creator
{
namespace sight::data
{

ModelSeries1::ModelSeries1()
{
    m_classname = "::sight::data::ModelSeries";
    m_version   = "1";
}

// ----------------------------------------------------------------------------

ModelSeries1::~ModelSeries1()
{
}

// ----------------------------------------------------------------------------

ModelSeries1::ModelSeries1( const ModelSeries1& cpy ) :
    Series1(cpy)
{
}

// ----------------------------------------------------------------------------

atoms::Object::sptr ModelSeries1::create()
{
    atoms::Object::sptr modelSeries = this->Series1::create();
    ::fwAtomsPatch::helper::Object helper(modelSeries);

    helper.addAttribute("reconstruction_db", atoms::Sequence::New());

    return modelSeries;
}

} // namespace sight::data
} // namespace creator
} // namespace fwStructuralPatch
