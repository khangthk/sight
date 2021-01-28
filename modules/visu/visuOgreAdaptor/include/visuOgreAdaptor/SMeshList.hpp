/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

#include <queue>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows a list of mesh.
 *
 * @section Slots Slots
 * - \b add(): add a new mesh at the transformation matrix position.
 * - \b clear(): clear all meshes.
 * - \b updateVisibility(bool): sets whether the mesh is to be seen or not.
 * - \b toggleVisibility(): toggle whether the mesh is shown or not.
 * - \b show(): shows the mesh.
 * - \b hide(): hides the mesh.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SMeshList">
        <inout key="mesh" uid="..." />
        <in key="transform" uid="..." />
        <in key="texture" uid="..." />
        <config layer="default" capacity="50" drop="1" textureAlpha="false" />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b texture [::fwData::Image]: image displayed as a texture on the mesh
 * - \b transform [::fwData::TransformationMatrix3D]: Sight transform matrix.
 *
 * @subsection In-Out In-Out
 * - \b mesh [::fwData::Mesh]: adapted mesh. It can not be a read-only data because we may generate normals or add some
 *      fields.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the list's layer.
 * - \b capacity (optional, int, default=50): maximum capacity of the list.
 * - \b drop (optional, int, default=1): defines the ratio of matrices to display: (1/drop).
 * - \b textureAlpha (optional, bool, default=false): generates alpha value for the texture if the image contains only
 * 1 or 3 channels. It may be slower.
 */
class VISUOGREADAPTOR_CLASS_API SMeshList final : public ::fwRenderOgre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMeshList, ::fwRenderOgre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SMeshList() noexcept;

    /// Destroys the service.
    VISUOGREADAPTOR_API ~SMeshList() noexcept override;

protected:

    /// Configures the adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// Initializes the adaptor.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposal to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::TransformationMatrix3D::s_MODIFIED_SIG of s_TRANSFORM_INOUT to s_ADD_MESH_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    VISUOGREADAPTOR_API void updating() override;

    /// Unregisters all services.
    /// @see clear()
    VISUOGREADAPTOR_API void stopping() override;

    /**
     * @brief Sets the list visibility.
     * @param _visible the visibility status of the list.
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    /// Stores all services and data needed to add a new mesh.
    struct MeshInstance
    {
        ::fwData::TransformationMatrix3D::sptr m_matrix { nullptr };
        ::fwData::Image::sptr m_image { nullptr };
        ::fwRenderOgre::IAdaptor::sptr m_transform { nullptr };
        ::fwRenderOgre::IAdaptor::sptr m_mesh { nullptr };
        ::fwRenderOgre::IAdaptor::sptr m_texture { nullptr };
        bool m_isEnabled{ false }; ///< set to true when the instance has been added once
    };

    /// Adds a new mesh at the input matrix position.
    void add();

    /// Clear all meshes.
    void clear();

    /// Stores all registered meshes instances.
    std::vector< MeshInstance > m_meshes;

    /// Defines the current index mesh to update in the mesh vector.
    unsigned int m_meshCount { 0 };

    /// Defines the maximum capacity of the meshes list.
    unsigned int m_capacity { 50 };

    /// Defines the number of matrices to drop before adding a new adaptor.
    unsigned int m_dropData { 1 };

    /// Counts the number of received signal to add a data, then drop the signal if (m_dropCount%m_dropData != 0).
    unsigned int m_dropCount { 0 };

    /// Generates alpha value for the texture if the image contains only 1 or 3 channels. It may be slower.
    bool m_generateAlpha{ false };

};

} // namespace visuOgreAdaptor.