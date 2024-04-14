/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <ui/__/action.hpp>

namespace sight::module::debug::action
{

/// Implements an action that dump all buffer (if possible)
class dump_all : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(dump_all, sight::ui::action);

    /// Does nothing
    dump_all() noexcept;

    /// Does nothing
    ~dump_all() noexcept override;

protected:

    /// Calls classic action methods to configure
    void configuring() override;

    /// Calls classic action methods to start
    void starting() override;

    /// Dump all unlocked array/image etc
    void updating() override;

    /// Calls classic action methods to stop
    void stopping() override;
};

} // namespace sight::module::debug::action
