/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "core/tools/TypeKeyTypeMapping.hpp"

namespace sight::core::tools
{

// FIX UNSIGNED
template<>
bool isMapping<std::uint8_t>(const Type& type)
{
    return type == Type::UINT8;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint16_t>(const Type& type)
{
    return type == Type::UINT16;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint32_t>(const Type& type)
{
    return type == Type::UINT32;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::uint64_t>(const Type& type)
{
    return type == Type::UINT64;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int8_t>(const Type& type)
{
    return type == Type::INT8;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int16_t>(const Type& type)
{
    return type == Type::INT16;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int32_t>(const Type& type)
{
    return type == Type::INT32;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<std::int64_t>(const Type& type)
{
    return type == Type::INT64;
}

// FLOATING PRECISION
template<>
bool isMapping<float>(const Type& type)
{
    return type == Type::FLOAT;
}

//------------------------------------------------------------------------------

template<>
bool isMapping<double>(const Type& type)
{
    return type == Type::DOUBLE;
}

} // namespace sight::core::tools
