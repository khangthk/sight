/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"
#include "modules/viz/scene3d/adaptor/STransform.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <data/Material.hpp>
#include <data/PointList.hpp>

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/Mesh.hpp>
#include <viz/scene3d/Text.hpp>

#include <OGRE/OgreEntity.h>

namespace sight::data
{

class Material;

}
namespace sight::data
{

class Mesh;

}

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a point list using billboards generated by a geometry shader.
 *
 * This class handles the display of a data::PointList or a data::Mesh. Both are exclusive you can only specify
 * one of the two.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether the points are visible or not.
 * - \b toggleVisibility(): Toggle whether the points are visible or not.
 * - \b show(): shows the video.
 * - \b hide(): hides the video.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SPointList" >
        <in key="pointList" uid="..." />
        <config layer="..." transform="..." textureName="..." radius="1.0" fontSource="DejaVuSans.ttf" fontSize="16"
               labelColor="#0xFFFFFF" visible="true" fixedSize="false" queryFlags="0x40000000" displayLabel="false"/>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b pointList [sight::data::PointList] (optional): point list to display.
 * - \b mesh [sight::data::Mesh] (optional): point based mesh to display. If the mesh contains any topology, it will be
 *      ignored and only raw vertices will be displayed. or add some fields.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 * - \b autoresetcamera (optional, true/false, default=true): reset the camera when this mesh is modified, "true" or
 *"false".
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the STransform adaptor.
 *      Either of the following (whether a material is configured in the XML scene or not) :
 * - \b materialTemplate (optional, string, default='Billboard_Default'): the name of the base Ogre material for the
 *      internally created SMaterial.
 * - \b textureName (optional, string, default=""): the name of the Ogre texture that the mesh will use.
 * - \b radius (optional, float, default=1.f): billboard radius.
 * - \b displayLabel (optional, bool, default=false): display the label points (default = false).
 * - \b labelColor (optional, hexadecimal, default=0xFFFFFF): color of the label in hexadecimal.
 * - \b color (optional, hexadecimal, default=#FFFFFFFF): color of the texture in hexadecimal.
 * - \b fixedSize (optional, bool, default=false): if true, the billboard will have a fixed size in screen space.
 * - \b queryFlags (optional, uint32, default=0x40000000): Picking flags. Points can be picked by pickers with a
 *      matching mask.
 * - \b visible (optional, bool, default=true): the pointlist visibility at start.
 * - \b fontSource (optional, string, default=DejaVuSans.ttf): TrueType font (*.ttf) source file.
 * - \b fontSize (optional, unsigned int, default=16): font size in points.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SPointList final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SPointList, sight::viz::scene3d::IAdaptor);

    /// Creates the adaptor, sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API SPointList() noexcept;

    /// Destroys Ogre resource.
    MODULE_VIZ_SCENE3D_API ~SPointList() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates a mesh in the default Ogre resource group.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::PointList::s_POINT_ADDED_SIG of s_POINTLIST_INPUT to s_UPDATE_SLOT
     * Connect data::PointList::s_POINT_REMOVED_SIG of s_POINTLIST_INPUT to s_UPDATE_SLOT
     * Connect data::PointList::s_MODIFIED_SIG of s_POINTLIST_INPUT to s_UPDATE_SLOT
     * Connect data::Mesh::s_VERTEX_MODIFIED_SIG of s_MESH_INPUT to s_UPDATE_SLOT
     * Connect data::Mesh::s_MODIFIED_SIG of s_MESH_INPUT to s_UPDATE_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the generated mesh.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the point list visibility.
     * @param _visible the visibility status of the point list.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /**
     * @brief Get the point list visibility.
     * @return True if the point list is visible.
     */
    bool getVisibility() const;

    /**
     * @brief Updates the point list from a point list, checks if color, number of vertices have changed, and updates
     * them.
     * @param _pointList point list used for the update.
     */
    void updateMesh(const data::PointList::csptr& _pointList);

    /**
     * @brief Updates the point list from a mesh, checks if color, number of vertices have changed, and updates them.
     * @param _mesh mesh used for the update.
     */
    void updateMesh(const data::Mesh::csptr& _mesh);

    /**
     * @brief Instantiates a new material adaptor.
     */
    module::viz::scene3d::adaptor::SMaterial::sptr createMaterialService(const std::string& _meshId);

    /// Associates a new SMaterial to the managed SPointList.
    /// With this method, SPointList is responsible for creating a SMaterial.
    void updateMaterialAdaptor(const std::string& _meshId);

    /**
     * @brief Attaches a node in the scene graph.
     * @param _node node to attach.
     */
    void attachNode(Ogre::MovableObject* _node);

    /// Detaches and destroy @ref m_entity from the scene graph.
    void detachAndDestroyEntity();

    /**
     * @brief Creates all the labels and attach them to the sceneNode vector.
     * @param _pointList point list used to retreive each point informations.
     */
    void createLabel(const data::PointList::csptr& _pointList);

    /// Destroys all the labels and delete them from the sceneNode vector.
    void destroyLabel();

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera {true};

    /// Defines whether the material was set by the user or not.
    bool m_customMaterial {false};

    /// Contains the node in the scene graph.
    Ogre::Entity* m_entity {nullptr};

    /// Contains the material attached to the mesh.
    module::viz::scene3d::adaptor::SMaterial::sptr m_materialAdaptor {nullptr};

    /// Contains the Ogre material related to the mesh.
    data::Material::sptr m_material {nullptr};

    /// Defines the attached material's name.
    std::string m_materialTemplateName {"Billboard_Default"};

    /// Defines the attached texture adaptor UID.
    std::string m_textureName;

    /// Contains the mesh support used to render the pointlist.
    sight::viz::scene3d::Mesh::sptr m_meshGeometry {nullptr};

    /// Defines the billboards radius.
    float m_radius {1.f};

    /// Defines if label numbers are displayed.
    bool m_displayLabel {false};

    /// Contains the RGB color for the label point color.
    data::Color::sptr m_labelColor {nullptr};

    /// Defines the mask for picking requests.
    std::uint32_t m_queryFlags {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Stores label of each point.
    std::vector<sight::viz::scene3d::Text*> m_labels;

    /// Stores label points nodes.
    std::vector<Ogre::SceneNode*> m_nodes;

    /// Contains the scene node where all of our manual objects are attached.
    Ogre::SceneNode* m_sceneNode {nullptr};

    /// Defines the TrueType font source file.
    std::string m_fontSource {"DejaVuSans.ttf"};

    /// Defines the font size in points.
    std::size_t m_fontSize {16};

    static constexpr std::string_view s_POINTLIST_INPUT = "pointList";
    static constexpr std::string_view s_MESH_INPUT      = "mesh";

    data::ptr<data::PointList, data::Access::in> m_pointList {this, s_POINTLIST_INPUT, true, true};
    data::ptr<data::Mesh, data::Access::in> m_mesh {this, s_MESH_INPUT, true, true};
};

//------------------------------------------------------------------------------

inline bool SPointList::getVisibility() const
{
    return m_entity ? m_entity->getVisible() : m_isVisible;
}

} // namespace sight::module::viz::scene3d::adaptor.
