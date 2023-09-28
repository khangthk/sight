/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <core/crypto/base64.hpp>

#include <io/zip/ArchiveReader.hpp>
#include <io/zip/ArchiveWriter.hpp>

#include <boost/property_tree/ptree.hpp>

#include <optional>

namespace sight::io::session::Helper
{

constexpr static auto s_Value {"Value"};
constexpr static auto s_Version {".version"};

/// Convenience function to read a version from a tree.
/// Optionally checks the version number, when minVersion or maxVersion > 0.
/// @param[in] tree boost property tree where version is stored
/// @param[in] minVersion the minimum valid version
/// @param[in] maxVersion the maximum valid version
template<typename T>
inline static int readVersion(
    const boost::property_tree::ptree& tree,
    const int minVersion = 0,
    const int maxVersion = 0
)
{
    // Add a version number. Not mandatory, but could help for future release
    const int version = tree.get<int>(T::classname() + s_Version, -1);

    SIGHT_THROW_IF(
        T::classname() << " deserialization is not implemented for version '" << version << "'.",
        (minVersion > 0 && minVersion > version) || (maxVersion > 0 && maxVersion < version)
    );

    return version;
}

/// Convenience function to write a version number in the property tree
/// @param[inout] tree boost property tree where the version must be stored
/// @param[in] version the version number to store
template<typename T>
inline static void writeVersion(boost::property_tree::ptree& tree, const int version = 1)
{
    // Add a version number. Not mandatory, but could help for future release
    tree.put(T::classname() + s_Version, std::to_string(version));
}

/// Convenience function to safely read strings from a tree
/// @param[in] tree boost property tree where string data are stored
/// @param[in] key the string data key
inline static std::string readString(
    const boost::property_tree::ptree& tree,
    const std::string& key,
    const std::optional<std::string>& default_value = std::nullopt
)
{
    try
    {
        const auto& base64 = tree.get<std::string>(key);
        return core::crypto::from_base64(base64);
    }
    catch(...)
    {
        if(default_value)
        {
            return *default_value;
        }

        throw;
    }
}

/// Convenience function to safely put strings into a tree
/// @param[inout] tree boost property tree where string data must be stored
/// @param[in] key the string data key
/// @param[in] value the string data
inline static void writeString(
    boost::property_tree::ptree& tree,
    const std::string& key,
    const std::string& value
)
{
    const auto& base64 = core::crypto::to_base64(value);
    tree.put(key, base64);
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] object the object to cast to type T
template<typename T>
inline static typename T::sptr safe_cast(sight::data::Object::sptr object)
{
    const auto& casted = std::dynamic_pointer_cast<T>(object);

    SIGHT_THROW_IF(
        "Object '"
        << (object ? object->get_classname() : sight::data::Object::classname())
        << "' is not a '"
        << T::classname()
        << "'",
        casted == nullptr
    );

    return casted;
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] object the object to cast to type T
template<typename T>
inline static typename T::sptr cast_or_create(sight::data::Object::sptr object)
{
    if(object)
    {
        return safe_cast<T>(object);
    }

    return std::make_shared<T>();
}

/// Convenience function to cast and check an object
/// Mainly to factorize error management
/// @param[in] object the object to cast to type T
template<typename T>
inline static typename T::csptr safe_cast(sight::data::Object::csptr object)
{
    const auto& casted = std::dynamic_pointer_cast<const T>(object);

    SIGHT_THROW_IF(
        "Object '"
        << (object ? object->get_classname() : sight::data::Object::classname())
        << "' is not a '"
        << T::classname()
        << "'",
        casted == nullptr
    );

    return casted;
}

/// Generic serialization function
/// @param[in,out] archive output archive where to write binary files to
/// @param[in,out] tree output tree where to write trivial class members to
/// @param[in] object the object to serialize
/// @param[out] children the list of child objects belonging to object
/// @param[in] password (optional) the password to use if encryption is enabled
template<typename T>
inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto& casted = safe_cast<T>(object);

    // Add a version number. Not mandatory, but could help for future release
    writeVersion<T>(tree, 1);

    tree.put(s_Value, casted->getValue());
}

/// Generic deserialization function
/// @param[in] archive the archive where to read binary files
/// @param[in] tree the tree where to read trivial object members
/// @param[in] children a map of data object that belongs to the object
/// @param[in,out] object the object to deserialize using the archive, the tree and the children map
/// @param[in] password (optional) the password used for encryption
template<typename T>
inline static typename T::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    const auto& casted = cast_or_create<T>(object);

    // Check version number. Not mandatory, but could help for future release
    readVersion<T>(tree, 0, 1);

    // Assign the value
    casted->setValue(tree.get<typename T::ValueType>(s_Value));

    return casted;
}

} // namespace sight::io::session::Helper
