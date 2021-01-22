/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "fwServices/config.hpp"
#include "fwServices/IParametersService.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>

namespace fwServices
{

/**
 * @brief   This interface defines operator service API.

 * @section Signals Signals
 * - \b computed() : Emitted when something was computed.
 */
class FWSERVICES_CLASS_API IOperator : public ::fwServices::IParametersService
{

public:

    fwCoreServiceMacro(IOperator, ::fwServices::IParametersService)

    FWSERVICES_API static const core::com::Signals::SignalKeyType s_COMPUTED_SIG;
    typedef core::com::Signal<void ()> ComputedSignalType;

protected:

    /// IOperator constructor.
    FWSERVICES_API IOperator();

    /// IOperator destructor.
    FWSERVICES_API virtual ~IOperator();

    ComputedSignalType::sptr m_sigComputed; ///< Signal emitted when operator is computed.
};

}
