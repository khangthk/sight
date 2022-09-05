/************************************************************************
 *
 * Copyright (C) 2015-2022 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"

#include <viz/scene3d/detail/IResourceManager.hpp>

#include <boost/noncopyable.hpp>

namespace sight::viz::scene3d
{

/**
 * @brief Represents and manages a transfer function from a GPU point of view
 */
template<class OBJECT, class RESOURCE, class IMPL>
class VIZ_SCENE3D_CLASS_API IResource : public boost::noncopyable
{
public:

    using sptr = std::shared_ptr<IMPL>;
    using wptr = std::weak_ptr<IMPL>;
    using uptr = std::unique_ptr<IMPL>;

    virtual ~IResource() = default;

    /// Update the TF resource according to the transfer function data.
    virtual void update() = 0;

    /// Return the Ogre resource.
    [[nodiscard]] Ogre::SharedPtr<RESOURCE> get() const;

protected:

    /// Shared resource
    Ogre::SharedPtr<RESOURCE> m_resource;
};

//-----------------------------------------------------------------------------

template<class OBJECT, class RESOURCE, class IMPL>
inline Ogre::SharedPtr<RESOURCE> IResource<OBJECT, RESOURCE, IMPL>::get() const
{
    return m_resource;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
