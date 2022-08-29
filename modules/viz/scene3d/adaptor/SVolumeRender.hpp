/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/config.hpp"

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/IGraphicsWorker.hpp>
#include <viz/scene3d/interactor/ClippingBoxInteractor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/vr/IllumAmbientOcclusionSAT.hpp>
#include <viz/scene3d/vr/PreIntegrationTable.hpp>
#include <viz/scene3d/vr/RayTracingVolumeRenderer.hpp>

#include <OGRE/OgreTexture.h>

#include <array>
#include <vector>

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor displays a volume rendering.
 *
 * @section Slots Slots
 * - \b newImage(): called when a new image is loaded.
 * - \b updateImage(): called when the image is updated.
 * - \b toggleWidgets(bool): toggles widget visibility.
 * - \b bufferImage(): called when the image buffer is modified, copies it into the texture buffer.
 * - \b updateVisibility(bool): shows or hides the volume.
 * - \b toggleVisibility(): toggle whether the volume is shown or not.
 * - \b show(): shows the volume.
 * - \b hide(): hides the volume.
 * - \b updateClippingBox(): updates the cropping widget from the clipping matrix.
 * - \b setBoolParameter(bool, string): calls a bool parameter slot according to the given key.
 *  - preIntegration: toggles the pre-integration.
 *  - ambientOcclusion: toggles the ambient occlusion.
 *  - colorBleeding: toggles the color bleeding.
 *  - shadows: toggles soft shadows.
 *  - widgets: toggles the clipping box widget's visibility.
 * - \b setIntParameter(int, string): calls an int parameter slot according to the given key.
 *  - sampling: sets the number of volume samples used by the renderer. More samples yield more details but slow down
 *    rendering.
 *  - opacityCorrection: sets the volume opacity correction factor.
 *  - satSizeRatio: sets the SAT ratio and computes it again with the new corresponding size.
 *  - satShellsNumber: sets the number of SAT shells and compute the SAT.
 *  - satShellRadius: sets the SAT shell radius and computes the SAT.
 *  - satConeAngle: sets the SAT cone angle and computes the SAT.
 *  - satConeSamples: sets the SAT cone samples number and computes the SAT.
 * - \b setDoubleParameter(double, string): calls a double parameter slot according to the given key.
 *  - aoFactor: sets the ambient occlusion factor and computes the SAT.
 *  - colorBleedingFactor: sets the color bleeding factor and computes the SAT.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SVolumeRender" >
        <in key="image" uid="..." autoConnect="true" />
        <in key="tf" uid="..." />
        <inout key="clippingMatrix" uid="..." />
        <config layer="default"
                samples="1024" preintegration="true" dynamic="false" ao="false" colorBleeding="false" shadows="false"
                satSizeRatio="0.25" satShells="3" satShellRadius="7" satConeAngle="0.1" satConeSamples="50"
                aoFactor="0.5" colorBleedingFactor="0.5" autoresetcamera="true" transform="..."/>
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::Image]: input volume data.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection In-Out In-Out
 * - \b clippingMatrix [sight::data::Matrix4]: matrix used to clip the volume.
 *
 * @subsection Configuration Configuration
 * - \b layer (mandatory, string): id of the layer where this adaptor applies.
 * - \b samples (optional, unsigned int, default=512): maximum number of samples per ray or number of slices.
 * - \b preintegration (optional, true/false, default=false): use pre-integration.
 * - \b dynamic (optional, bool, default=false): enables background buffering for dynamic images.
 * - \b widgets (optional, true/false, default=true): display VR widgets.
 * - \b priority (optional, int, default=2): interaction priority of the widget.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 * - \b ao (optional, bool, default=false): ambient occlusion usage.
 * - \b colorBleeding (optional, bool, default=false): color bleeding usage.
 * - \b shadows (optional, bool, default=false): soft shadows usage.
 * - \b satSizeRatio (optional, float, default=0.25): ratio used to determine the size of the SAT regarding of the
 *      associated image size.
 * - \b satShells (optional, int, default=3): number of shells used to compute the volume illumination from the SAT.
 * - \b satShellRadius (optional, int, default=7): radius of the shells used to compute the volume illumination
 *      from the SAT.
 * - \b satConeAngle (optional, float, default=0.1): angle used to define the soft shadows cones.
 * - \b satConeSamples (optional, float, default=50): number of samples along the soft shadows cones.
 * - \b aoFactor (optional, double, default=1.0): factor used to weight the ambient occlusion.
 * - \b colorBleedingFactor (optional, double, default=1.0): factor used to weight the color bleeding.
 * - \b autoresetcamera (optional, true/false, default=true): reset the camera at image update to view the whole volume.
 * - \b transform (optional, string, default=""): transform applied to the adaptor's scene node.
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SVolumeRender final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SVolumeRender, sight::viz::scene3d::IAdaptor);

    /// Creates slots.
    MODULE_VIZ_SCENE3D_API SVolumeRender() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SVolumeRender() noexcept override;

protected:

    // Slot keys
    static inline const sight::core::com::Slots::SlotKeyType s_NEW_IMAGE_SLOT            = "newImage";
    static inline const sight::core::com::Slots::SlotKeyType s_BUFFER_IMAGE_SLOT         = "bufferImage";
    static inline const sight::core::com::Slots::SlotKeyType s_UPDATE_IMAGE_SLOT         = "updateImage";
    static inline const sight::core::com::Slots::SlotKeyType s_TOGGLE_WIDGETS_SLOT       = "toggleWidgets";
    static inline const sight::core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
    static inline const sight::core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
    static inline const sight::core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
    static inline const sight::core::com::Slots::SlotKeyType s_UPDATE_CLIPPING_BOX_SLOT  = "updateClippingBox";
    static inline const sight::core::com::Slots::SlotKeyType s_UPDATE_TF_SLOT            = "updateTF";

    ///@brief Internal wrapper holding config defines.
    struct config
    {
        static inline const std::string AUTORESET_CAMERA      = "autoresetcamera";
        static inline const std::string PREINTEGRATION        = "preintegration";
        static inline const std::string DYNAMIC               = "dynamic";
        static inline const std::string WIDGETS               = "widgets";
        static inline const std::string PRIORITY              = "priority";
        static inline const std::string LAYER_ORDER_DEPENDANT = "layerOrderDependant";
        static inline const std::string SAMPLES               = "samples";
        static inline const std::string SAT_SIZE_RATIO        = "satSizeRatio";
        static inline const std::string SAT_SHELLS            = "satShells";
        static inline const std::string SAT_SHELL_RADIUS      = "satShellRadius";
        static inline const std::string SAT_CONE_ANGLE        = "satConeAngle";
        static inline const std::string SAT_CONE_SAMPLES      = "satConeSamples";
        static inline const std::string AO_FACTOR             = "aoFactor";
        static inline const std::string COLOR_BLEEDING_FACTOR = "colorBleedingFactor";
        static inline const std::string AO                    = "ao";
        static inline const std::string COLOR_BLEEDING        = "colorBleeding";
        static inline const std::string SHADOWS               = "shadows";
    };

    /// Internal wrapper holding the inital config
    struct config_data_t
    {
        using sat_parameters_t     = sight::viz::scene3d::vr::IllumAmbientOcclusionSAT::sat_parameters_t;
        using shadows_parameters_t = sight::viz::scene3d::vr::RayTracingVolumeRenderer::shadows_parameters_t;

        /// Enables whether the camera must be auto reset when a mesh is updated or not.
        bool camera_autoreset {false};

        /// Enables preintegration.
        bool preintegration {false};

        /// Enables dynamic buffering.
        bool dynamic {false};

        /// Sets the node visible or not.
        bool visible {false};

        /// Interactor priority.
        int priority {2};

        /// Indicates the the clipping box interactor layer is order-dependant.
        bool order_dependent {false};

        /// Sampling rate.
        std::uint16_t samples {512};

        /// SAT parameters.
        sat_parameters_t sat {};

        ///Shadows parameters
        shadows_parameters_t shadows {};
    };

    /// Internal wrapper holding object keys
    struct objects
    {
        static constexpr std::string_view IMAGE_IN              = "image";
        static constexpr std::string_view VOLUME_TF_IN          = "tf";
        static constexpr std::string_view CLIPPING_MATRIX_INOUT = "clippingMatrix";
    };

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Starts the service and initializes scene objects.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_IMAGE_INOUT to s_NEW_IMAGE_SLOT
     * Connect data::Image::s_BUFFER_MODIFIED_SIG of s_IMAGE_INOUT to s_BUFFER_IMAGE_SLOT
     * Connect data::Image::s_MODIFIED_SIG of s_CLIPPING_MATRIX_INOUT to s_UPDATE_CLIPPING_BOX_SLOT
     */
    MODULE_VIZ_SCENE3D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Does nothing.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Cleans up scene objects.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the volume visibility.
     * @param _visible the visibility status of the volume.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

    ///@brief Configuration loaded.
    config_data_t m_config {};

private:

    /// Defines volume rendering effects.
    enum class VREffectType
    {
        VR_AMBIENT_OCCLUSION,
        VR_COLOR_BLEEDING,
        VR_SHADOWS
    };

    /// Updates the transfer function applied to the volume.
    void updateVolumeTF();

    /// Sets a new image.
    void newImage();

    /// Updates renderer and the GPU volume texture with the new input image data.
    void updateImage();

    /// Starts a parallel task to copy the updated image buffer into the texture buffer.
    void bufferImage();

    /**
     * @brief Updates the sampling.
     * @param _nbSamples number of sample.
     *
     * @pre _nbSamples must fit in a 16 bit unsigned int.
     */
    void updateSampling(unsigned _nbSamples);

    /**
     * @brief Sets the opacity correction.
     * @param _opacityCorrection value of the opacity correction.
     */
    void updateOpacityCorrection(unsigned _opacityCorrection);

    /**
     * @brief Sets the ambient occlusion factor.
     * @param _aoFactor value of the ambient occlusion factor.
     */
    void updateAOFactor(float _aoFactor);

    /**
     * @brief Sets the color bleeding factor.
     * @param _colorBleedingFactor value of the color bleeding factor.
     */
    void updateColorBleedingFactor(float _colorBleedingFactor);

    /**
     * @brief Sets the SAT size ratio.
     * @param _sizeRatio value of the SAT size ratio.
     */
    void updateSatSizeRatio(unsigned _sizeRatio);

    /**
     * @brief Sets the SAT shells number.
     * @param _shellsNumber the number of shells used by the SAT.
     */
    void updateSatShellsNumber(unsigned _shellsNumber);

    /**
     * @brief Sets the SAT shells radius.
     * @param _shellRadius the shells radius used by the SAT.
     */
    void updateSatShellRadius(unsigned _shellRadius);

    /**
     * @brief Sets the SAT cone angle.
     * @param _coneAngle the cone angle size of the SAT. Cones ares used to compute soft shadows.
     */
    void updateSatConeAngle(float _coneAngle);

    /**
     * @brief Sets the SAT cone samples.
     * @param _nbConeSamples the cone sample number of the SAT. Cones ares used to compute soft shadows.
     */
    void updateSatConeSamples(unsigned _nbConeSamples);

    /**
     * @brief Enables/disables the pre integration table.
     * @param _preintegration state of the pre integration.
     */
    void togglePreintegration(bool _preintegration);

    /**
     * @brief Enables/disables the ambient occlusion.
     * @param _ambientOcclusion state of the ambient occlusion.
     */
    void toggleAmbientOcclusion(bool _ambientOcclusion);

    /**
     * @brief Enables/disables the color bleeding.
     * @param _colorBleeding state of the color bleeding.
     */
    void toggleColorBleeding(bool _colorBleeding);

    /**
     * @brief Enables/disables the volume self-shadowing.
     * @param _shadows state of soft shadows computation.
     */
    void toggleShadows(bool _shadows);

    /**
     * @brief Displays/Hides the widget.
     * @param _visible state of the widget visibility.
     */
    void toggleWidgets(bool _visible);

    /**
     * @brief Sets the focal distance.
     * @param _focalDistance value of the focal distance.
     */
    void setFocalDistance(int _focalDistance);

    /**
     * @brief Updates a bool parameter.
     * @param _val new value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - preIntegration: toggles the pre-integration.
     *  - ambientOcclusion: toggles the ambient occlusion.
     *  - colorBleeding: toggles the color bleeding.
     *  - shadows: toggles soft shadows.
     *  - widgets: toggles the clipping box widget's visibility.
     *
     * @see togglePreintegration(bool)
     * @see toggleAmbientOcclusion(bool)
     * @see toggleColorBleeding(bool)
     * @see toggleShadows(bool)
     * @see toggleWidgets(bool)
     */
    void setBoolParameter(bool _val, std::string _key);

    /**
     * @brief Updates a int parameter.
     * @param _val New value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - sampling: Sets the number of volume samples used by the renderer. More samples yield more details but slow
     *    down rendering.
     *  - opacityCorrection: sets the volume opacity correction factor.
     *  - satSizeRatio: sets the SAT ratio and computes it again with the new corresponding size.
     *  - satShellsNumber: sets the number of SAT shells and compute the SA.
     *  - satShellRadius: sets the SAT shell radius and computes the SAT.
     *  - satConeAngle: sets the SAT cone angle and computes the SAT.
     *  - satConeSamples: sets the SAT cone samples number and computes the SAT.
     *
     * @see updateSampling(int)
     * @see updateOpacityCorrection(int)
     * @see updateSatSizeRatio(int)
     * @see updateSatShellsNumber(int)
     * @see updateSatShellRadius(int)
     * @see updateSatConeAngle(int)
     * @see updateSatConeSamples(int)
     */
    void setIntParameter(int _val, std::string _key);

    /**
     * @brief Updates a double parameter.
     * @param _val the new value of the parameter.
     * @param _key the key of the parameter. Following keys are accepted:
     *  - aoFactor: sets the ambient occlusion factor and computes the SAT.
     *  - colorBleedingFactor: sets the color bleeding factor and computes the SAT.
     *
     * @see updateAOFactor(double)
     * @see updateColorBleedingFactor(double)
     */
    void setDoubleParameter(double _val, std::string _key);

    /// Creates widgets and connects its slots to interactor signals.
    void createWidget();

    /// Removes the widgets from the interactor and deletes it.
    void destroyWidget();

    /**
     * @brief Updates or creates the illumination volume according to the given VR effect.
     * @param _vrEffect volume rendering effects.
     * @param _enable enable that effect or not.
     */
    void toggleVREffect(VREffectType _vrEffect, bool _enable);

    /// Updates the clipping box position from the inout clipping matrix.
    void updateClippingBox();

    /// Updates the inout clipping matrix from the clipping box positions.
    void updateClippingTM3D();

    ///Prevents concurrent access on certain operations (texture update, etc.)
    std::mutex m_mutex;

    /// Implements a simple GPU ray-tracing renderer.
    std::unique_ptr<sight::viz::scene3d::vr::RayTracingVolumeRenderer> m_volumeRenderer {nullptr};

    /// Fills the incoming image texture in a parallel thread.
    std::unique_ptr<sight::viz::scene3d::IGraphicsWorker> m_bufferingWorker;

    /// Stores the scene manager.
    Ogre::SceneManager* m_sceneManager {nullptr};

    /// Stores the scene node of the volume.
    Ogre::SceneNode* m_volumeSceneNode {nullptr};

    /// Stores the widgets used for clipping.
    std::shared_ptr<sight::viz::scene3d::interactor::ClippingBoxInteractor> m_widget;

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, objects::IMAGE_IN, true};
    sight::data::ptr<sight::data::TransferFunction, sight::data::Access::in> m_tf {this, objects::VOLUME_TF_IN, true};
    sight::data::ptr<sight::data::Matrix4, sight::data::Access::inout> m_clippingMatrix
    {
        this,
        objects::CLIPPING_MATRIX_INOUT,
        true,
        true
    };
};

} // namespace sight::module::viz::scene3d::adaptor.
