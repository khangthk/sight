/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/Helper.hpp"

#include <data/Point.hpp>

namespace sight::io::session::detail::Point
{

constexpr static auto s_X {"X"};
constexpr static auto s_Y {"Y"};
constexpr static auto s_Z {"Z"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto point = Helper::safeCast<data::Point>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Point>(tree, 1);

    const auto& coordinate = point->getCoord();
    tree.put(s_X, coordinate[0]);
    tree.put(s_Y, coordinate[1]);
    tree.put(s_Z, coordinate[2]);
}

//------------------------------------------------------------------------------

inline static data::Point::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto point = Helper::safeCast<data::Point>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Point>(tree, 0, 1);

    const std::array<double, 3> coordinates = {
        tree.get<double>(s_X),
        tree.get<double>(s_Y),
        tree.get<double>(s_Z)
    };

    point->setCoord(coordinates);

    return point;
}

} // namespace sight::io::session::detail::Point
