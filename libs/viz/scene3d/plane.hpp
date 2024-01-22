/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "viz/scene3d/config.hpp"

#include <data/helper/medical_image.hpp>

#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMesh.h>
#include <OGRE/OgreSceneManager.h>
#include <OGRE/OgreTechnique.h>
#include <OGRE/OgreTexture.h>

#include <string>

namespace Ogre
{

class SceneNode;

} // namespace Ogre

namespace sight::viz::scene3d
{

/**
 * @brief Manages a plane mesh on which a slice texture will be applied.
 */
class VIZ_SCENE3D_CLASS_API plane
{
public:

    using orientation_mode = data::helper::medical_image::orientation_t;

    /// Defines the texture filtering mode.
    enum class filter_t : std::uint8_t
    {
        none,
        linear,
        anisotropic
    };

    using sptr = std::shared_ptr<plane>;

    /**
     * @brief Creates a plane, instantiates its material. Call @ref plane::update() to create its geometry.
     * @param _negato_id unique identifier of the negato.
     * @param _parent_scene_node parent node where attach the plane.
     * @param _scene_manager the Ogre scene manager.
     * @param _tex texture to apply on the plane.
     * @param _filtering filtering method used to apply the texture.
     * @param _display_border display a border around the negato plane.
     * @param _display_other_planes display a line indicating the location of the two other planes.
     * @param _entity_opacity opacity of the entity.
     */
    VIZ_SCENE3D_API plane(
        const core::tools::id::type& _negato_id,
        Ogre::SceneNode* _parent_scene_node,
        Ogre::SceneManager* _scene_manager,
        viz::scene3d::texture::sptr _tex,
        filter_t _filtering,
        bool _display_border       = true,
        bool _display_other_planes = true,
        float _entity_opacity      = 1.0F
    );

    /// Cleans ogre resources.
    VIZ_SCENE3D_API ~plane();

    /**
     * @brief Instantiates the plane mesh and entity.
     * @param _enable_transparency used true to enable the opacity.
     */
    VIZ_SCENE3D_API void update(
        orientation_mode _orientation,
        const Ogre::Vector3& _spacing,
        const Ogre::Vector3& _origin,
        bool _enable_transparency
    );

    /**
     * @brief Handles the slice plane move.
     *     - in 2D, it will convert the position in unit floating value and call the fragment shader.
     *     - in 3D, it will also move the scene node in space.
     * @param _slices_index the image slices indexes.
     */
    VIZ_SCENE3D_API void change_slice(const std::array<float, 3>& _slices_index);

    /**
     * @brief Sets the plane's opacity.
     * @param _f the opacity value
     *
     * @pre _f must fit between 0 and 1.
     */
    VIZ_SCENE3D_API void set_entity_opacity(float _f);

    /**
     * @brief Shows/hides the plane in the scene.
     * @param _visible use true to show the plane.
     */
    VIZ_SCENE3D_API void set_visible(bool _visible);

    /**
     * @brief Adds or updates the texture containing the transfer function data in the negato passes.
     * @param _tf_texture the TF texture.
     */
    VIZ_SCENE3D_API void set_tf_data(const viz::scene3d::transfer_function& _tf_texture);

    /// Gets the image axis orthogonal to the plane.
    [[nodiscard]] VIZ_SCENE3D_API orientation_mode get_orientation_mode() const;

    /// Gets the movable object created by this class.
    [[nodiscard]] VIZ_SCENE3D_API const Ogre::MovableObject* get_movable_object() const;

    /**
     * @brief Sets the picking flags.
     * @param _flags the flags use for the picking.
     */
    VIZ_SCENE3D_API void set_query_flags(std::uint32_t _flags);

    /// Sets this object's render queue group and render priority.
    VIZ_SCENE3D_API void set_render_queuer_group_and_priority(std::uint8_t _group_id, std::uint16_t _priority);

    /// Compute two cross lines that intersect at the given position, according to the plane orientation.
    VIZ_SCENE3D_API std::array<Ogre::Vector3, 4> compute_cross(
        const Ogre::Vector3& _center,
        const Ogre::Vector3& _image_origin
    ) const;

private:

    /// Sets the plane's original position.
    void update_position();

    /// Sets the dimensions for the related members, and also creates a movable plane to instantiate the entity.
    Ogre::MovablePlane set_dimensions(const Ogre::Vector3& _spacing);

    struct line_shape
    {
        /// Contains the manual object that represent borders.
        Ogre::ManualObject* shape {nullptr};

        /// Contains the material used to display borders.
        Ogre::MaterialPtr material {nullptr};

        /// Is it displayed?
        bool enabled {true};
    };

    /// Optional border
    line_shape m_border;

    /// Lines representing the slices of the two other orthogonal planes
    line_shape m_slices_cross;

    /// Defines the filtering type for this plane.
    filter_t m_filtering {filter_t::anisotropic};

    /// Defines the orientation mode of the plane.
    orientation_mode m_orientation {orientation_mode::x_axis};

    /// Contains the plane on which we will apply a texture.
    Ogre::MeshPtr m_slice_plane;

    /// Defines the origin position of the slice plane according to the source image's origin.
    Ogre::Vector3 m_origin {Ogre::Vector3::ZERO};

    /// Contains the plane material.
    Ogre::MaterialPtr m_tex_material {nullptr};

    /// Contains the texture.
    viz::scene3d::texture::sptr m_texture;

    /// Contains the scene manager containing the plane.
    Ogre::SceneManager* m_scene_manager {nullptr};

    /// Defines a strings needed to initialize mesh, scenenode, etc.
    std::string m_slice_plane_name;

    /// Defines the entity name. used to recover this from the Ogre entityManager.
    std::string m_entity_name;

    /// Defines the scene node name used to recover the scene node from it's name.
    std::string m_scene_node_name;

    /// Contains the scene node on which we will attach the mesh.
    Ogre::SceneNode* m_plane_scene_node {nullptr};

    /// Contains the parent scene node.
    Ogre::SceneNode* m_parent_scene_node {nullptr};

    /// Defines the entity's size in the X,Y,Z axis.
    Ogre::Vector3 m_size {0.F};

    /// Defines the opacity applied to the entity.
    float m_entity_opacity {1.F};
};

//------------------------------------------------------------------------------

inline plane::orientation_mode plane::get_orientation_mode() const
{
    return m_orientation;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d
