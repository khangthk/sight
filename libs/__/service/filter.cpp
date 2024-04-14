/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "service/filter.hpp"

#include <core/com/signal.hxx>

namespace sight::service
{

//-----------------------------------------------------------------------------

const core::com::signals::key_t filter::COMPUTED_SIG = "computed";

//-----------------------------------------------------------------------------

filter::filter() :
    m_sig_computed(new_signal<computed_signal_t>(COMPUTED_SIG))
{
}

//-----------------------------------------------------------------------------

filter::~filter()
= default;

//-----------------------------------------------------------------------------

} // namespace sight::service
