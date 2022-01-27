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

#include "viz/scene2d/config.hpp"

#include <service/IXMLParser.hpp>

namespace sight::viz::scene2d::data::parser
{

/**
 * @brief This service parse an hexa viewport value and set it to the viewport data.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <object uid="..." type="data::Viewport" >
        <config x="-500" y="-1.1" width="500" height="1.2" />
    </object>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b x: set the x coordinate of the top left viewport corner
 * - \b y: set the y coordinate of the top left viewport corner
 * - \b width: set the width of the viewport
 * - \b height: set the height of the viewport
 */
class VIZ_SCENE2D_CLASS_API Viewport final : public service::IXMLParser
{
public:

    /// Creates default attributes and methods (New(), ::sptr, ...).
    SIGHT_DECLARE_SERVICE(Viewport, service::IXMLParser);

    /**
     * @brief Parse a viewport XML configuration.
     * @param _obj viewport in which apply the configuration.
     */
    VIZ_SCENE2D_API void createConfig(core::tools::Object::sptr _obj) override;
};

} // namespace sight::viz::scene2d::data::parser