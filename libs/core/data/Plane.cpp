/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/Plane.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

SIGHT_REGISTER_DATA(sight::data::Plane);

namespace sight::data
{

const core::com::Signals::SignalKeyType Plane::s_SELECTED_SIG = "selected";

//------------------------------------------------------------------------------

Plane::Plane(data::Object::Key /*unused*/)
{
    m_vPoints[0] = data::Point::New();
    m_vPoints[1] = data::Point::New();
    m_vPoints[2] = data::Point::New();

    newSignal<SelectedSignalType>(s_SELECTED_SIG);
}

//------------------------------------------------------------------------------

void Plane::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vPoints = other->m_vPoints;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Plane::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_vPoints[0] = data::Object::copy(other->m_vPoints[0], cache);
    m_vPoints[1] = data::Object::copy(other->m_vPoints[1], cache);
    m_vPoints[2] = data::Object::copy(other->m_vPoints[2], cache);

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

void Plane::setValue(data::Point::sptr _point1, data::Point::sptr _point2, data::Point::sptr _point3)
{
    m_vPoints[0] = _point1;
    m_vPoints[1] = _point2;
    m_vPoints[2] = _point3;
}

//------------------------------------------------------------------------------

bool Plane::operator==(const Plane& other) const noexcept
{
    if(m_isIntersection != other.m_isIntersection
       || !core::tools::is_equal(m_vPoints, other.m_vPoints))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Plane::operator!=(const Plane& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
