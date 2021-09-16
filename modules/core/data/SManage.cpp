/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "SManage.hpp"

#include <data/Composite.hpp>
#include <data/Exception.hpp>
#include <data/helper/Composite.hpp>
#include <data/helper/Field.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/helper/Vector.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

namespace sight::module::data
{

const core::com::Slots::SlotKeyType SManage::s_ADD_SLOT               = "add";
const core::com::Slots::SlotKeyType SManage::s_ADD_COPY_SLOT          = "addCopy";
const core::com::Slots::SlotKeyType SManage::s_ADD_OR_SWAP_SLOT       = "addOrSwap";
const core::com::Slots::SlotKeyType SManage::s_SWAP_OBJ_SLOT          = "swapObj";
const core::com::Slots::SlotKeyType SManage::s_REMOVE_SLOT            = "remove";
const core::com::Slots::SlotKeyType SManage::s_REMOVE_IF_PRESENT_SLOT = "removeIfPresent";
const core::com::Slots::SlotKeyType SManage::s_CLEAR_SLOT             = "clear";

const service::IService::KeyType s_COMPOSITE_INOUT    = "composite";
const service::IService::KeyType s_VECTOR_INOUT       = "vector";
const service::IService::KeyType s_SERIESDB_INOUT     = "seriesDB";
const service::IService::KeyType s_FIELD_HOLDER_INOUT = "fieldHolder";
const service::IService::KeyType s_OBJECT_INOUT       = "object";

//-----------------------------------------------------------------------------

SManage::SManage() noexcept
{
    newSlot(s_ADD_SLOT, &SManage::add, this);
    newSlot(s_ADD_COPY_SLOT, &SManage::addCopy, this);
    newSlot(s_ADD_OR_SWAP_SLOT, &SManage::addOrSwap, this);
    newSlot(s_SWAP_OBJ_SLOT, &SManage::swap, this);
    newSlot(s_REMOVE_SLOT, &SManage::remove, this);
    newSlot(s_REMOVE_IF_PRESENT_SLOT, &SManage::removeIfPresent, this);
    newSlot(s_CLEAR_SLOT, &SManage::clear, this);
}

//-----------------------------------------------------------------------------

SManage::~SManage() noexcept
{
}

//-----------------------------------------------------------------------------

void SManage::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    m_compositeKey = config.get("compositeKey", "");
    m_fieldName    = config.get("field", "");
}

//-----------------------------------------------------------------------------

void SManage::starting()
{
}

//-----------------------------------------------------------------------------

void SManage::stopping()
{
}

//-----------------------------------------------------------------------------

void SManage::updating()
{
}

//-----------------------------------------------------------------------------

void SManage::add()
{
    internalAdd(false);
}

//------------------------------------------------------------------------------

void SManage::addCopy()
{
    internalAdd(true);
}

//-----------------------------------------------------------------------------

void SManage::addOrSwap()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.addOrSwap(m_fieldName, obj.get_shared());
        helper.notify();
    }
    else
    {
        const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());
        const auto vector    = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared());
        const auto seriesDB  = std::dynamic_pointer_cast<sight::data::SeriesDB>(container.get_shared());

        if(composite)
        {
            sight::data::helper::Composite helper(composite);
            if(composite->find(m_compositeKey) == composite->end())
            {
                helper.add(m_compositeKey, obj.get_shared());
            }
            else
            {
                helper.swap(m_compositeKey, obj.get_shared());
            }

            helper.notify();
        }
        else if(vector)
        {
            auto iter = std::find(vector->begin(), vector->end(), obj.get_shared());
            if(iter == vector->end())
            {
                sight::data::helper::Vector helper(vector);
                helper.add(obj.get_shared());
                helper.notify();
            }

            SIGHT_WARN_IF("Object already exists in the Vector, does nothing.", iter != vector->end());
        }
        else
        {
            SIGHT_ASSERT("Source object is assumed to be a SeriesDB", seriesDB);
            sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj.get_shared());
            SIGHT_ASSERT("Target object is a SeriesDB, so object must be a Series", series);

            auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
            if(iter == seriesDB->end())
            {
                sight::data::helper::SeriesDB helper(*seriesDB);
                helper.add(series);
                helper.notify();
            }

            SIGHT_WARN_IF("Object already exists in the SeriesDB, does nothing.", iter != seriesDB->end());
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::swap()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();
    SIGHT_ASSERT("Object is missing.", obj);

    const auto container = m_container.lock();
    const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.swap(m_fieldName, obj.get_shared());
        helper.notify();
    }
    else if(composite)
    {
        sight::data::helper::Composite helper(composite);
        helper.swap(m_compositeKey, obj.get_shared());
        helper.notify();
    }
    else
    {
        SIGHT_WARN("'swap' slot is only managed for 'composite' or 'fieldHolder'");
    }
}

//-----------------------------------------------------------------------------

void SManage::remove()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.remove(m_fieldName);
        helper.notify();
    }
    else
    {
        const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());
        const auto vector    = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared());
        const auto seriesDB  = std::dynamic_pointer_cast<sight::data::SeriesDB>(container.get_shared());

        if(composite)
        {
            sight::data::helper::Composite helper(composite);
            helper.remove(m_compositeKey);
            helper.notify();
        }
        else if(vector)
        {
            SIGHT_ASSERT("Object is missing.", obj);
            sight::data::helper::Vector helper(vector);
            helper.remove(obj.get_shared());
            helper.notify();
        }
        else
        {
            SIGHT_ASSERT("Source object is assumed to be a SeriesDB", seriesDB);

            SIGHT_ASSERT("Object is missing.", obj);
            sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj.get_shared());
            SIGHT_ASSERT("Target object is a SeriesDB, so object must be a Series", series);

            sight::data::helper::SeriesDB helper(*seriesDB);
            helper.remove(series);
            helper.notify();
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::removeIfPresent()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto obj = m_object.lock();

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        try
        {
            helper.remove(m_fieldName);
            helper.notify();
        }
        catch(sight::data::Exception&)
        {
            // Silently ignore the exception which means the field was not present
        }
    }
    else
    {
        const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());
        const auto vector    = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared());
        const auto seriesDB  = std::dynamic_pointer_cast<sight::data::SeriesDB>(container.get_shared());

        if(composite)
        {
            sight::data::helper::Composite helper(composite);
            if(composite->find(m_compositeKey) != composite->end())
            {
                helper.remove(m_compositeKey);
                helper.notify();
            }
        }
        else if(vector)
        {
            SIGHT_ASSERT("Object is missing.", obj);
            auto iter = std::find(vector->begin(), vector->end(), obj.get_shared());
            if(iter != vector->end())
            {
                sight::data::helper::Vector helper(vector);
                helper.remove(obj.get_shared());
                helper.notify();
            }

            SIGHT_WARN_IF("Object does not exist in the Vector, does nothing.", iter == vector->end());
        }
        else
        {
            SIGHT_ASSERT("Source object is assumed to be a SeriesDB", seriesDB);

            SIGHT_ASSERT("Object is missing.", obj);
            sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj.get_shared());
            SIGHT_ASSERT("Target object is a SeriesDB, so object must be a Series", series);

            auto iter = std::find(seriesDB->begin(), seriesDB->end(), series);
            if(iter != seriesDB->end())
            {
                sight::data::helper::SeriesDB helper(*seriesDB);
                helper.remove(series);
                helper.notify();
            }

            SIGHT_WARN_IF("Object does not exist in the SeriesDB, does nothing.", iter == seriesDB->end());
        }
    }
}

//-----------------------------------------------------------------------------

void SManage::clear()
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.clear();
        helper.notify();
    }
    else
    {
        const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());
        const auto vector    = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared());
        const auto seriesDB  = std::dynamic_pointer_cast<sight::data::SeriesDB>(container.get_shared());
        if(composite)
        {
            sight::data::helper::Composite helper(composite);
            helper.clear();
            helper.notify();
        }
        else if(vector)
        {
            sight::data::helper::Vector helper(vector);
            helper.clear();
            helper.notify();
        }
        else
        {
            SIGHT_ASSERT("Source object is assumed to be a SeriesDB", seriesDB);
            sight::data::helper::SeriesDB helper(*seriesDB);
            helper.clear();
            helper.notify();
        }
    }
}

//------------------------------------------------------------------------------

void SManage::internalAdd(bool _copy)
{
    SIGHT_ASSERT("Service is not started", this->isStarted());

    const auto object = m_object.lock();
    SIGHT_ASSERT("Object is missing.", object);

    auto obj = object.get_shared();
    if(_copy)
    {
        obj = sight::data::Object::copy(obj);
    }

    const auto container = m_container.lock();

    if(!m_fieldName.empty())
    {
        sight::data::helper::Field helper(container.get_shared());
        helper.add(m_fieldName, obj);
        helper.notify();
    }
    else
    {
        const auto composite = std::dynamic_pointer_cast<sight::data::Composite>(container.get_shared());
        const auto vector    = std::dynamic_pointer_cast<sight::data::Vector>(container.get_shared());
        const auto seriesDB  = std::dynamic_pointer_cast<sight::data::SeriesDB>(container.get_shared());

        if(composite)
        {
            sight::data::helper::Composite helper(composite);
            helper.add(m_compositeKey, obj);
            helper.notify();
        }
        else if(vector)
        {
            sight::data::helper::Vector helper(vector);
            helper.add(obj);
            helper.notify();
        }
        else if(seriesDB)
        {
            SIGHT_ASSERT("Source object is assumed to be a SeriesDB", seriesDB);
            sight::data::Series::sptr series = sight::data::Series::dynamicCast(obj);
            SIGHT_ASSERT("Target object is a SeriesDB, so object must be a Series.", series);
            sight::data::helper::SeriesDB helper(*seriesDB);
            helper.add(series);
            helper.notify();
        }
    }
}

//-----------------------------------------------------------------------------

} // sight::module::data