#ifndef __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
#define __VISUOGREADAPTOR_SVOLUMERENDER_HPP__

#include <array>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwComEd/helper/MedicalImageAdaptor.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>
#include <fwRenderOgre/PreIntegrationTable.hpp>
#include <fwRenderOgre/RayTracingVolumeRenderer.hpp>
#include <fwRenderOgre/SliceVolumeRenderer.hpp>
#include <fwRenderOgre/TransferFunction.hpp>
#include <fwRenderOgre/ui/VRWidget.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgrePolygon.h>
#include <OGRE/OgreTexture.h>

#include <vector>

#include "visuOgreAdaptor/config.hpp"

namespace visuOgreAdaptor
{
class VISUOGREADAPTOR_CLASS_API SVolumeRender : public ::fwRenderOgre::IAdaptor,
                                                public ::fwRenderOgre::ITransformable,
                                                public ::fwComEd::helper::MedicalImageAdaptor
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SVolumeRender)(::fwRenderOgre::IAdaptor) );

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWIMAGE_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_NEWSAMPLING_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SVolumeRender() throw();

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SVolumeRender() throw();

protected:

    /// Starts the service initializes scene objects.
    VISUOGREADAPTOR_API virtual void doStart() throw ( ::fwTools::Failed );

    /// Cleans up memory.
    VISUOGREADAPTOR_API virtual void doStop() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doSwap() throw ( ::fwTools::Failed );

    /// Does nothing.
    VISUOGREADAPTOR_API virtual void doUpdate() throw ( ::fwTools::Failed );

    /// Configure volume rendering service.
    VISUOGREADAPTOR_API virtual void doConfigure() throw ( ::fwTools::Failed );

    /// Slot called on TF update.
    VISUOGREADAPTOR_API virtual void updatingTFPoints();

    /// Slot called on TF window update.
    VISUOGREADAPTOR_API virtual void updatingTFWindowing(double window, double level);

    /// Get object/service connections.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    /// Camera listener, updates proxy geometry on camera move.
    class CameraMotionListener : public Ogre::Camera::Listener
    {
    public:

        /// Constructor.
        CameraMotionListener(SVolumeRender *parent);

        /// Called before scene render.
        void cameraPreRenderScene(Ogre::Camera *);

    private:
        /// The volume to update.
        ::visuOgreAdaptor::SVolumeRender *m_parent;

    };

    typedef std::vector< ::Ogre::Vector3 > Polygon;

    /// Slot called when a new image is loaded.
    void newImage();

    /// Slot called when the sampling is changed.
    void samplingChanged(int nbSamples);

    /// Creates widgets and connects its slots to interactor signals.
    void initWidgets();

    /// Rendering mode.
    enum
    {
        VR_MODE_SLICE,
        VR_MODE_RAY_TRACING
    } m_renderingMode;

    /// Renders the volume.
    ::fwRenderOgre::IVolumeRenderer *m_volumeRenderer;

    /// 3D Image texture.
    ::Ogre::TexturePtr m_3DOgreTexture;

    /// TF texture used for rendering.
    ::fwRenderOgre::TransferFunction m_gpuTF;

    /// Pre-integration table.
    ::fwRenderOgre::PreIntegrationTable m_preIntegrationTable;

    /// This object's scene manager.
    ::Ogre::SceneManager *m_sceneManager;

    /// This object's scene node.
    ::Ogre::SceneNode *m_volumeSceneNode;

    /// Camera used for rendering.
    ::Ogre::Camera *m_camera;

    /// Widgets used for clipping.
    ::fwRenderOgre::ui::VRWidget::sptr m_widgets;

    /// Sampling rate.
    uint16_t m_nbSlices;

    /// Use pre-integration.
    bool m_preIntegratedRendering;
};

} // visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SVOLUMERENDER_HPP__
