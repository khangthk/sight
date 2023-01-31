/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/scene3d/ITransformable.hpp"

#include "viz/scene3d/helper/Scene.hpp"
#include "viz/scene3d/IAdaptor.hpp"

namespace sight::viz::scene3d
{

const std::string ITransformable::s_TRANSFORM_CONFIG = "config.<xmlattr>.transform";

//------------------------------------------------------------------------------

void ITransformable::setTransformId(viz::scene3d::SRender::OgreObjectIdType _id)
{
    m_transformId = _id;
}

//------------------------------------------------------------------------------

viz::scene3d::SRender::OgreObjectIdType ITransformable::getTransformId() const
{
    return m_transformId;
}

//------------------------------------------------------------------------------

Ogre::SceneNode* ITransformable::getTransformNode() const
{
    return m_transformNode;
}

//------------------------------------------------------------------------------

Ogre::SceneNode* ITransformable::getOrCreateTransformNode(
    Ogre::SceneNode* const _parentNode
)
{
    SIGHT_ASSERT("Transform id is not set.", !m_transformId.empty());
    auto* transformNode = viz::scene3d::helper::Scene::getNodeById(m_transformId, _parentNode);
    if(transformNode == nullptr)
    {
        transformNode = _parentNode->createChildSceneNode(m_transformId);
    }

    // assign m_transform to the retrieved/created node.
    m_transformNode = transformNode;

    return m_transformNode;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d
