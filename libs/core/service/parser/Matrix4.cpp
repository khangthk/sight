/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "service/parser/Matrix4.hpp"

#include "service/macros.hpp"

#include <data/Matrix4.hpp>

#include <cmath>

namespace sight::service::parser
{

//------------------------------------------------------------------------------

void Matrix4::updating()
{
    SIGHT_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void Matrix4::createConfig(core::tools::Object::sptr _obj)
{
    data::Matrix4::sptr matrix = data::Matrix4::dynamicCast(_obj);
    SIGHT_ASSERT("matrix not instanced", matrix);

    for(const auto& elem : m_cfg.get_child("object"))
    {
        if(elem.first == "matrix")
        {
            const auto input = elem.second.get_value<std::string>();
            std::istringstream inputString(input);

            double coef = NAN;
            for(unsigned int k = 0 ; k < 16 ; k++)
            {
                inputString >> coef;
                matrix->getCoefficients()[k] = coef;
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service::parser
