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

#include <data/Color.hpp>
#include <data/StructureTraits.hpp>

namespace sight::io::session
{

namespace detail::StructureTraits
{

constexpr static auto s_Type {"Type"};
constexpr static auto s_Class {"Class"};
constexpr static auto s_NativeExp {"NativeExp"};
constexpr static auto s_NativeGeometricExp {"NativeGeometricExp"};
constexpr static auto s_AttachmentType {"AttachmentType"};
constexpr static auto s_AnatomicRegion {"AnatomicRegion"};
constexpr static auto s_PropertyCategory {"PropertyCategory"};
constexpr static auto s_PropertyType {"PropertyType"};
constexpr static auto s_Category {"Category"};
constexpr static auto s_Categories {"Categories"};
constexpr static auto s_Color {"Color"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto structureTraits = Helper::safeCast<data::StructureTraits>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::StructureTraits>(tree, 1);

    // Serialize attributes
    Helper::writeString(tree, s_Type, structureTraits->getType());
    tree.put(s_Class, structureTraits->getClass());
    Helper::writeString(tree, s_NativeExp, structureTraits->getNativeExp());
    Helper::writeString(tree, s_NativeGeometricExp, structureTraits->getNativeGeometricExp());
    Helper::writeString(tree, s_AttachmentType, structureTraits->getAttachmentType());
    Helper::writeString(tree, s_AnatomicRegion, structureTraits->getAnatomicRegion());
    Helper::writeString(tree, s_PropertyCategory, structureTraits->getPropertyCategory());
    Helper::writeString(tree, s_PropertyType, structureTraits->getPropertyType());

    // Categories
    boost::property_tree::ptree categoriesTree;

    for(const auto& category : structureTraits->getCategories())
    {
        categoriesTree.add(s_Category, category);
    }

    tree.add_child(s_Categories, categoriesTree);

    // Color
    children[s_Color] = structureTraits->getColor();
}

//------------------------------------------------------------------------------

inline static data::StructureTraits::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto structureTraits = Helper::safeCast<data::StructureTraits>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::StructureTraits>(tree, 0, 1);

    // Deserialize attributes
    structureTraits->setType(Helper::readString(tree, s_Type));
    structureTraits->setClass(static_cast<data::StructureTraits::StructureClass>(tree.get<int>(s_Class)));
    structureTraits->setNativeExp(Helper::readString(tree, s_NativeExp));
    structureTraits->setNativeGeometricExp(Helper::readString(tree, s_NativeGeometricExp));
    structureTraits->setAttachmentType(Helper::readString(tree, s_AttachmentType));
    structureTraits->setAnatomicRegion(Helper::readString(tree, s_AnatomicRegion));
    structureTraits->setPropertyCategory(Helper::readString(tree, s_PropertyCategory));
    structureTraits->setPropertyType(Helper::readString(tree, s_PropertyType));

    // Categories
    auto& categories = structureTraits->getCategories();

    // Clearing is required in case the object is reused
    categories.clear();

    for(const auto& categoryTree : tree.get_child(s_Categories))
    {
        categories.push_back(static_cast<data::StructureTraits::Category>(categoryTree.second.get_value<int>()));
    }

    // Color
    structureTraits->setColor(std::dynamic_pointer_cast<data::Color>(children.at(s_Color)));

    return structureTraits;
}

} // namespace detail::StructureTraits

} // namespace sight::io
