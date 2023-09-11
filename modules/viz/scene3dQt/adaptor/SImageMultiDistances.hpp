/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3dQt/config.hpp"

#include <data/PointList.hpp>

#include <geometry/data/PointList.hpp>

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/interactor/IInteractor.hpp>
#include <viz/scene3d/IText.hpp>
#include <viz/scene3d/Material.hpp>

#include <Ogre.h>
#include <QPushButton>

#include <memory>
#include <optional>

namespace sight::module::viz::scene3dQt::adaptor
{

/**
 * @brief This adaptor displays distances retrieved from the image fields.
 *

 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SImageMultiDistances" autoConnect="true" >
        <inout key="image" uid="..." />
        <config fontSource="DejaVuSans.ttf" fontSize="32" radius="4.5" priority="2" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b image [sight::data::Image]: image containing the distance field.
 *
 * @subsection Configuration Configuration:
 * - \b fontSize (optional, int, default=16): font size in points.
 * - \b radius (optional, float, default=4.5): size of the distances spheres.
 * - \b interactive (optional, bool, default=true): enables interactions with distances.
 * - \b priority (optional, int, default=2): priority of the interactor.
 * - \b queryMask (optional, uint32, default=0xFFFFFFFF): mask used to filter out entities when the distance is auto
 *      snapped.
 * - \b distanceQueryFlags (optional, uint32, default=0x40000000): mask apply to distances spheres.
 *
 * @section Slots Slots
 * - \b removeAll(): remove all distances.
 * - \b removeDistance(): removes distances contained in the image from the scene manager.
 * - \b updateVisibilityFromField(): updates the visibility of distances from the field status.
 * - \b updateVisibility(bool): sets whether distances are shown or not.
 * - \b updateModifiedDistance(data::PointList::sptr): renders distances in other negato2d and negato3d adaptors.
 * - \b toggleVisibility(): toggles whether distances are shown or not.
 * - \b show(): shows distance.
 * - \b hide(): hides distance.
 *
 * @section Signals Signals
 * - \b toolDeactivate(): signal set when the tool is deactivated.
 *
 *
 */
class SImageMultiDistances final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::interactor::IInteractor
{
public:

    struct signals
    {
        using key_t = sight::core::com::Signals::SignalKeyType;
        static inline const key_t s_DEACTIVATE_DISTANCE_TOOL = "deactivateDistanceTool";

        using void_signal_t = sight::core::com::Signal<void ()>;
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SImageMultiDistances, sight::viz::scene3d::IAdaptor);

    /// Initialize slots.
    MODULE_VIZ_SCENE3DQT_API SImageMultiDistances() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3DQT_API ~SImageMultiDistances() noexcept override = default;

    /**
     * @brief Retrieves the picked distance and stores the result in m_pickedData.
     * @param _button mouse modifier.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3DQT_API void buttonPressEvent(MouseButton _button, Modifier _mod, int _x, int _y) override;

    /**
     * @brief Moves a distance stored in m_pickedData.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     */
    MODULE_VIZ_SCENE3DQT_API void mouseMoveEvent(
        MouseButton /*_button*/,
        Modifier _mod,
        int _x,
        int _y,
        int /*_dx*/,
        int /*_dy*/
    ) override;

    /// Resets m_pickedData.
    MODULE_VIZ_SCENE3DQT_API void buttonReleaseEvent(
        MouseButton _button,
        Modifier _mod,
        int _x,
        int _y
    ) override;

    /// catch escape to go out of add distance mode
    MODULE_VIZ_SCENE3DQT_API void keyPressEvent(int _key, Modifier /*_mods*/, int /*_mouseX*/, int /*_mouseY*/) final;

    /// Hides the contextual menu when the mouse wheel is used
    MODULE_VIZ_SCENE3DQT_API void wheelEvent(Modifier _mods, double _angleDelta, int _x, int _y) final;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3DQT_API void configuring() override;

    /// Adds the interactor to the layer and creates the material.
    MODULE_VIZ_SCENE3DQT_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     *
     * Connect data::Image::s_DISTANCE_REMOVED_SIG to s_REMOVE_DISTANCE_SLOT
     * Connect data::Image::s_DISTANCE_DISPLAYED_SIG to s_UPDATE_VISIBILITY_SLOT
     * Connect data::Image::s_MODIFIED_SIG to IService::slots::s_UPDATE
     */
    MODULE_VIZ_SCENE3DQT_API KeyConnectionsMap getAutoConnections() const override;

    /// Updates materials and all distances.
    MODULE_VIZ_SCENE3DQT_API void updating() override;

    /// Removes the interactor from the layer and destroys Ogre resources.
    MODULE_VIZ_SCENE3DQT_API void stopping() override;

    /**
     * @brief Sets distances visibility.
     * @param _visible the visibility status of distances.
     */
    MODULE_VIZ_SCENE3DQT_API void setVisible(bool _visible) override;

private:

    /// Stores Ogre resources used to display a distance.
    /// Two spheres each attached to a node, a label to display millimeters,
    /// one line rendered with the depth check and a dashed line rendered without depth check.
    /// The point list is used to update each points when the interactor move a distance sphere,
    /// it's retrieve from the image via a field.
    struct DistanceData
    {
        data::PointList::sptr m_pointList;
        Ogre::SceneNode* m_node1 {};
        Ogre::ManualObject* m_sphere1 {};
        Ogre::SceneNode* m_node2 {};
        Ogre::ManualObject* m_sphere2 {};
        Ogre::ManualObject* m_line {};
        Ogre::ManualObject* m_dashedLine {};
        Ogre::SceneNode* m_labelNode {};
        sight::viz::scene3d::IText::sptr m_label;
    };

    /// Stores picking information.
    struct PickedData
    {
        DistanceData* m_data;
        bool m_first;
    };

    /// Map each distances to there related list ID.
    using DistanceMap = std::map<core::tools::fwID::IDType, DistanceData>;

    class DeleteBinButtonWhenFocusOut : public QObject
    {
    public:

        explicit DeleteBinButtonWhenFocusOut(SImageMultiDistances* sImageMultiDistances);
        bool eventFilter(QObject* o, QEvent* e) override;
        SImageMultiDistances* m_sImageMultiDistances;
    };

    /**
     * @brief Generates a color from a distance ID.
     * @param _id ID of the distance.
     * @return The generated color.
     */
    Ogre::ColourValue generateColor();

    /**
     * @brief Generates a dashed line in a Ogre::ManualObject.
     * @param _object Object where generate the dashed line.
     * @param _begin Begin position of the line.
     * @param _end End position of the line.
     * @param _thickness Thickness of dash.
     */
    static void generateDashedLine(
        Ogre::ManualObject* _object,
        const Ogre::Vector3& _begin,
        const Ogre::Vector3& _end,
        float _thickness
    );

    /**
     * @brief Gets the formatted string used to display the length of a distance.
     * @return The formatted string.
     */
    static std::string getLength(const Ogre::Vector3& /*_begin*/, const Ogre::Vector3& /*_end*/);

    /**
     * @brief Gets the normalized camera direction vector.
     * @return A vector representing the camera direction
     */
    static Ogre::Vector3 getCamDirection(const Ogre::Camera* /*_cam*/);

    /// Saves the created distances to image's field
    void updateImageDistanceField(data::PointList::sptr _pl);

    /// SLOT: Remove all distances
    void removeAll();

    /// Retrieves distances from the image and remove them from the scene.
    void removeDistances();

    /// Updates distances visibility from the image field.
    void updateVisibilityFromField();

    /// Activates the distance tool by changing the cursor and updating a boolean
    void activateDistanceTool(bool _activate);

    /**
     * @brief Gets the nearest picked position if there is one.
     * @param _x X screen coordinate.
     * @param _y Y screen coordinate.
     * @return The picked world coordinates.
     */
    std::optional<Ogre::Vector3> getNearestPickedPosition(int _x, int _y);

    /**
     * @brief Creates a distance and add it into m_distances.
     * @param _pl The point list used to create the distance.
     */
    void createDistance(data::PointList::sptr& _pl);

    /**
     * @brief Updates a distance.
     * @param _data Distance to update.
     * @param _begin New begin position.
     * @param _end New end position
     */
    void updateDistance(const DistanceData* _data, Ogre::Vector3 _begin, Ogre::Vector3 _end);

    /**
     * @brief Destroys a distance from its ID and remove it from m_distances.
     * @param _id ID of the distance.
     */
    void destroyDistance(core::tools::fwID::IDType _id);

    /// Slot: Allows visualizing the creation and/or modification of distances field of an Image in multiple
    /// adaptors(Negato2d/Negato3d) by displaying the distances synchronously.
    void updateModifiedDistance(data::PointList::sptr _pl);

    void setCursor(QCursor cursor);

    void removeDistance(data::PointList::sptr pl);

    /// Defines the radius of distances spheres.
    float m_distanceSphereRadius {3.5F};

    /// Defines the font size in points.
    std::size_t m_fontSize {12};

    /// Defines whether or not interactions are enabled with distances.
    bool m_interactive {true};

    /// Defines whether to start using the distance creation tool or not.
    bool m_toolActivated {false};

    /// Defines whether the distances are actually visible or not.
    bool m_visible {true};

    /// Defines whether a distance is in creation or not.
    bool m_creationMode {false};

    /// @brief  indicates if is over a distance
    bool m_isOverDistance {false};

    /// Defines the priority of the interactor.
    int m_priority {2};

    /// Counter to swap color at each new distance
    int m_colorIndex {0};

    /// Defines the current picked data, reset by buttonReleaseEvent(MouseButton, int, int).
    PickedData m_pickedData {nullptr, true};

    /// Defines the mask used to filter out entities when the distance is auto snapped.
    std::uint32_t m_queryMask {0xFFFFFFFF};

    /// Defines the mask used to filter distances, it optimizes the ray launched to retrieve the picked distance.
    std::uint32_t m_distanceQueryFlag {Ogre::SceneManager::ENTITY_TYPE_MASK};

    /// Defines the material name with no depth check for spheres.
    std::string m_sphereMaterialName;

    /// Defines the material name with depth check for lines.
    std::string m_lineMaterialName;

    /// Defines the material name with no depth check for dashed lines.
    std::string m_dashedLineMaterialName;

    /// Contains the material with no depth check for spheres.
    sight::viz::scene3d::Material::uptr m_sphereMaterial {nullptr};

    /// Contains the material with depth check for lines.
    sight::viz::scene3d::Material::uptr m_lineMaterial {nullptr};

    /// Contains the material with no depth check for dashed lines.
    sight::viz::scene3d::Material::uptr m_dashedLineMaterial {nullptr};

    /// Stores all generated distances.
    DistanceMap m_distances;
    std::vector<data::Point::sptr> m_points;

    QPushButton* m_binButton = nullptr;

    bool m_isAMouseMove = false;

    std::unique_ptr<DeleteBinButtonWhenFocusOut> m_eventFilter = nullptr;

    static constexpr std::string_view s_IMAGE_INOUT = "image";
    sight::data::ptr<sight::data::Image, sight::data::Access::inout> m_image {this, s_IMAGE_INOUT, true};
};

} // namespace sight::module::viz::scene3dQt::adaptor