/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "IHasParameters.hpp"

#include <core/com/Slots.hxx>

namespace sight::ui::base
{

//-----------------------------------------------------------------------------

const core::com::Slots::SlotKeyType IHasParameters::s_SET_PARAMETER_SLOT         = "setParameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_BOOL_PARAMETER_SLOT    = "setBoolParameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_COLOR_PARAMETER_SLOT   = "setColorParameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_DOUBLE_PARAMETER_SLOT  = "setDoubleParameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_DOUBLE2_PARAMETER_SLOT = "setDouble2Parameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_DOUBLE3_PARAMETER_SLOT = "setDouble3Parameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_INT_PARAMETER_SLOT     = "setIntParameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_INT2_PARAMETER_SLOT    = "setInt2Parameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_INT3_PARAMETER_SLOT    = "setInt3Parameter";
const core::com::Slots::SlotKeyType IHasParameters::s_SET_ENUM_PARAMETER_SLOT    = "setEnumParameter";

//-----------------------------------------------------------------------------

IHasParameters::IHasParameters(sight::core::com::Slots& _slots)
{
    _slots(s_SET_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setParameter, this));
    _slots(s_SET_BOOL_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setBoolParameter, this));
    _slots(s_SET_COLOR_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setColorParameter, this));
    _slots(s_SET_DOUBLE_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setDoubleParameter, this));
    _slots(s_SET_DOUBLE2_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setDouble2Parameter, this));
    _slots(s_SET_DOUBLE3_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setDouble3Parameter, this));
    _slots(s_SET_INT_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setIntParameter, this));
    _slots(s_SET_INT2_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setInt2Parameter, this));
    _slots(s_SET_INT3_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setInt3Parameter, this));
    _slots(s_SET_ENUM_PARAMETER_SLOT, core::com::newSlot(&IHasParameters::setEnumParameter, this));
}

//-----------------------------------------------------------------------------

IHasParameters::~IHasParameters()
= default;

//------------------------------------------------------------------------------

void IHasParameters::setParameter(sight::ui::base::parameter_t /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setBoolParameter(bool /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setColorParameter(std::array<std::uint8_t, 4> /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setDoubleParameter(double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setDouble2Parameter(double /*unused*/, double /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setDouble3Parameter(
    double /*unused*/,
    double /*unused*/,
    double /*unused*/,
    std::string /*unused*/
)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setIntParameter(int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setInt2Parameter(int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setInt3Parameter(int /*unused*/, int /*unused*/, int /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

void IHasParameters::setEnumParameter(std::string /*unused*/, std::string /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
