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

#pragma once

#include "data/container.hpp"
#include "data/object.hpp"

namespace sight::data
{

/// This class is an activity container
class DATA_CLASS_API vector final : public data::container<std::vector<object::sptr> >
{
public:

    SIGHT_DECLARE_CLASS(vector, container<vector::container_t>);

    /// Destructor / Assignment operators
    /// @{
    DATA_API ~vector() override = default;

    /// This will enable common collection constructors / assignment operators
    using container<vector::container_t>::container;
    using container<vector::container_t>::operator=;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const vector& _other) const noexcept;
    DATA_API bool operator!=(const vector& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;
};

} // namespace sight::data
