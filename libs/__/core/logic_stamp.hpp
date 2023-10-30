/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "core/base.hpp"
#include "core/hires_clock.hpp"

namespace sight::core
{

/**
 * @brief Provides a Logical timestamp system.
 *
 *
 */
class CORE_CLASS_API logic_stamp : public base_object
{
public:

    SIGHT_DECLARE_CLASS(logic_stamp, base_object);

    /**
     * @brief Type used in logical typestamp.
     */
    using logic_stamp_type = std::uint64_t;

    /**
     * @name Constructor/Destructor
     * @{ */

    logic_stamp()
    = default;

    /**  @} */

    /**
     * @brief Increment global Logical counter and copy it to this LogicStamp.
     */
    CORE_API void modified();

    /**
     * @return LogicStamp's current value
     */
    logic_stamp_type get_logic_stamp() const
    {
        return this->m_modified_logical_time;
    }

    /**
     * @brief Greater than operator for LogicStamp.
     *
     * @param ls LogicStamp to compare to
     */
    bool operator>(const logic_stamp& _ls) const
    {
        return this->m_modified_logical_time > _ls.m_modified_logical_time;
    }

    /**
     * @brief Lesser than operator for LogicStamp.
     *
     * @param ls LogicStamp to compare to
     */
    bool operator<(const logic_stamp& _ls) const
    {
        return this->m_modified_logical_time < _ls.m_modified_logical_time;
    }

    /**
     * @brief Cast operator for LogicStamp.
     */
    operator logic_stamp_type() const
    {
        return this->m_modified_logical_time;
    }

private:

    /**
     * @brief Stored logical time
     */
    logic_stamp_type m_modified_logical_time {0};
};

} //namespace sight::core
