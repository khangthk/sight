/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SAxis.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <fwCom/Slots.hxx>

#include <fwRenderOgre/helper/ManualObject.hpp>
#include <fwRenderOgre/helper/Scene.hpp>

#include <fwServices/macros.hpp>

#include <OgreNode.h>

namespace visuOgreAdaptor
{

const ::fwCom::Slots::SlotKeyType SAxis::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SAxis::s_TOGGLE_VISIBILITY_SLOT = "toggleVisibility";

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SAxis);

static const std::string s_LENGTH_CONFIG    = "length";
static const std::string s_BILLBOARD_CONFIG = "billboard";

//-----------------------------------------------------------------------------

SAxis::SAxis() noexcept :
    m_material(nullptr),
    m_length(50.f),
    m_isVisible(true)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SAxis::updateVisibility, this);
    newSlot(s_TOGGLE_VISIBILITY_SLOT, &SAxis::toggleVisibility, this);
}

//-----------------------------------------------------------------------------

SAxis::~SAxis() noexcept
{
}

//-----------------------------------------------------------------------------

void SAxis::updateVisibility(bool isVisible)
{
    m_isVisible = isVisible;
    this->updating();
}

//------------------------------------------------------------------------------

void SAxis::toggleVisibility()
{
    m_isVisible = !m_isVisible;
    this->updating();
}

//-----------------------------------------------------------------------------

bool SAxis::getVisibility() const
{
    return m_isVisible;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SAxis::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( ::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM, ::fwData::Object::s_MODIFIED_SIG,
                      s_UPDATE_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

void SAxis::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    // parsing transform or create an "empty" one
    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_CONFIG_TRANSFORM,
                                                            this->getID() + "_transform");

    this->setTransformId(transformId);
    m_length = config.get<float>(s_LENGTH_CONFIG, m_length);
}

//-----------------------------------------------------------------------------

void SAxis::starting()
{
    this->initialize();

    ::Ogre::SceneNode* rootSceneNode = this->getSceneManager()->getRootSceneNode();

    ::Ogre::SceneNode* transNode = ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);
    if (transNode == nullptr)
    {
        transNode = rootSceneNode->createChildSceneNode(this->getTransformId());
        transNode->setVisible(m_isVisible);
    }

    ::Ogre::SceneManager* sceneMgr = this->getSceneManager();

    xLine = sceneMgr->createManualObject(this->getID() + "_xline");
    yLine = sceneMgr->createManualObject(this->getID() + "_yline");
    zLine = sceneMgr->createManualObject(this->getID() + "_zline");

    xCone = sceneMgr->createManualObject(this->getID() + "_xCone");
    yCone = sceneMgr->createManualObject(this->getID() + "_yCone");
    zCone = sceneMgr->createManualObject(this->getID() + "_zCone");

    // set the material
    m_material = ::fwData::Material::New();

    ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();

    materialAdaptor->getMaterialFw()->setHasVertexColor(true);
    materialAdaptor->update();

    // Size
    const float cylinderLength = m_length - m_length/10;
    const float cylinderRadius = m_length/80;
    const float coneLength     = m_length - cylinderLength;
    const float coneRadius     = cylinderRadius*2;
    const unsigned sample      = 64;

    // Draw

    // X axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(xLine, materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Red),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* xLineNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_xLine");
    xLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(0.f, -coneRadius, -coneRadius),
                                                 ::Ogre::Vector3(m_length, coneRadius, coneRadius)));
    xLineNode->attachObject(xLine);
    xLineNode->pitch(::Ogre::Degree(90));

    // Y axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(yLine,
                                                         materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Green),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* yLineNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_yLine");
    yLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(-coneRadius, 0.f, -coneRadius),
                                                 ::Ogre::Vector3(coneRadius, m_length, coneRadius)));
    yLineNode->attachObject(yLine);
    yLineNode->roll(::Ogre::Degree(90));

    // Z axis
    ::fwRenderOgre::helper::ManualObject::createCylinder(zLine,
                                                         materialAdaptor->getMaterialName(),
                                                         ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
                                                         cylinderRadius,
                                                         cylinderLength,
                                                         sample);
    ::Ogre::SceneNode* zLineNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_zLine");
    yLine->setBoundingBox(::Ogre::AxisAlignedBox(::Ogre::Vector3(-coneRadius, -coneRadius, 0.f),
                                                 ::Ogre::Vector3(coneRadius, coneRadius, m_length)));
    zLineNode->attachObject(zLine);
    zLineNode->yaw(::Ogre::Degree(-90));

    // X cone
    ::fwRenderOgre::helper::ManualObject::createCone(xCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Red),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* xConeNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_xCone");
    xConeNode->attachObject(xCone);
    xConeNode->translate(cylinderLength, 0.f, 0.f);

    // Y cone
    ::fwRenderOgre::helper::ManualObject::createCone(yCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Green),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* yConeNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_yCone");
    yConeNode->attachObject(yCone);
    yConeNode->translate(0.f, cylinderLength, 0.f);
    yConeNode->roll(::Ogre::Degree(90));

    // Z cone
    ::fwRenderOgre::helper::ManualObject::createCone(zCone, materialAdaptor->getMaterialName(),
                                                     ::Ogre::ColourValue(::Ogre::ColourValue::Blue),
                                                     coneRadius,
                                                     coneLength,
                                                     sample);
    ::Ogre::SceneNode* zConeNode = transNode->createChildSceneNode(
        this->getTransformId() + "_" + this->getID() + "_zCone");
    zConeNode->attachObject(zCone);
    zConeNode->translate(0.f, 0.f, cylinderLength);
    zConeNode->yaw(::Ogre::Degree(-90));

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SAxis::stopping()
{
    ::Ogre::SceneManager* sceneMgr   = this->getSceneManager();
    ::Ogre::SceneNode* rootSceneNode = sceneMgr->getRootSceneNode();
    ::Ogre::SceneNode* transNode     =
        ::fwRenderOgre::helper::Scene::getNodeById(this->getTransformId(), rootSceneNode);

    if(transNode != nullptr)
    {
        transNode->removeAndDestroyAllChildren();
    }

    sceneMgr->destroyManualObject(xLine);
    sceneMgr->destroyManualObject(yLine);
    sceneMgr->destroyManualObject(zLine);

    sceneMgr->destroyManualObject(xCone);
    sceneMgr->destroyManualObject(yCone);
    sceneMgr->destroyManualObject(zCone);

    this->unregisterServices();
    m_material.reset();
}

//-----------------------------------------------------------------------------

} //visuOgreAdaptor
