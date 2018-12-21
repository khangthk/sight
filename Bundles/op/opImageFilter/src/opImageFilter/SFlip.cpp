/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "opImageFilter/SFlip.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <imageFilterOp/Flipper.hpp>

namespace opImageFilter
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opImageFilter::SFlip, ::fwData::Image);

static const ::fwServices::IService::KeyType s_IMAGE_IN  = "source";
static const ::fwServices::IService::KeyType s_IMAGE_OUT = "target";

const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXISX_SLOT = "flipAxisX";
const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXISY_SLOT = "flipAxisY";
const ::fwCom::Slots::SlotKeyType SFlip::s_FLIP_AXISZ_SLOT = "flipAxisZ";

//------------------------------------------------------------------------------

SFlip::SFlip()
{
    // Do not flip any axis by default
    m_flipAxes.push_back(false);
    m_flipAxes.push_back(false);
    m_flipAxes.push_back(false);

    // Initialize the slots
    newSlot(s_FLIP_AXISX_SLOT, &SFlip::flipAxisX, this);
    newSlot(s_FLIP_AXISY_SLOT, &SFlip::flipAxisY, this);
    newSlot(s_FLIP_AXISZ_SLOT, &SFlip::flipAxisZ, this);
}

//------------------------------------------------------------------------------

SFlip::~SFlip()
{

}

//------------------------------------------------------------------------------

void SFlip::configuring()
{

}

//------------------------------------------------------------------------------

void SFlip::starting()
{

}

//------------------------------------------------------------------------------

void SFlip::updating()
{
    ::fwData::Image::csptr inImg = this->getInput< ::fwData::Image >(s_IMAGE_IN);

    ::fwData::mt::ObjectReadLock inImLock(inImg);

    SLM_ASSERT("No 'imageIn' found !", inImg);

    ::fwData::Image::sptr outImg = ::fwData::Image::New();
    ::imageFilterOp::Flipper::flip(inImg, outImg, m_flipAxes);

    this->setOutput(s_IMAGE_OUT, outImg);
}

//------------------------------------------------------------------------------

void SFlip::stopping()
{

}

//------------------------------------------------------------------------------

void SFlip::flipAxisX()
{
    m_flipAxes[0] = !(m_flipAxes[0]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisY()
{
    m_flipAxes[1] = !(m_flipAxes[1]);
    this->updating();
}

//------------------------------------------------------------------------------

void SFlip::flipAxisZ()
{
    m_flipAxes[2] = !(m_flipAxes[2]);
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFlip::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_IMAGE_IN, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_IN, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

} // namespace opImageFilter
