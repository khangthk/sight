/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "viz/scene3d/TransferFunction.hpp"

#include <viz/scene3d/detail/TFManager.hpp>

namespace sight::viz::scene3d
{

//-----------------------------------------------------------------------------

TransferFunction::TransferFunction(const data::TransferFunction::csptr& _tf, const std::string& suffixId)
{
    m_resource = detail::TFManager::get()->instantiate(_tf, suffixId);
}

//-----------------------------------------------------------------------------

TransferFunction::~TransferFunction()
{
    if(m_resource)
    {
        detail::TFManager::get()->release(m_resource);
    }
}

//-----------------------------------------------------------------------------

void TransferFunction::update()
{
    m_tfWindow = viz::scene3d::detail::TFManager::get()->load(m_resource).second;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
