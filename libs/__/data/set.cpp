/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "set.hpp"

#include "data/container.hpp"
#include "data/registry/macros.hpp"

#include <core/com/signal.hxx>

SIGHT_REGISTER_DATA(sight::data::set);

namespace sight::data
{

//------------------------------------------------------------------------------

void set::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

bool set::operator==(const set& _other) const noexcept
{
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool set::operator!=(const set& _other) const noexcept
{
    return base_class_t::operator!=(_other);
}

//------------------------------------------------------------------------------

void set::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const set>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>")) + " to " + get_classname()
        ),
        !other
    );

    base_class_t::deep_copy(other, _cache);
}

} // namespace sight::data
