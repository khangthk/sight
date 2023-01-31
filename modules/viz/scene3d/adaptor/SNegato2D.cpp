/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/SNegato2D.hpp"

#include <core/com/Signals.hpp>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <service/macros.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/Utils.hpp>

#include <OgreSceneNode.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

const core::com::Slots::SlotKeyType s_SLICETYPE_SLOT        = "sliceType";
const core::com::Slots::SlotKeyType s_SLICEINDEX_SLOT       = "sliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_TF_SLOT = "updateTF";

static const core::com::Slots::SlotKeyType s_UPDATE_SLICES_FROM_WORLD = "updateSlicesFromWorld";

static const core::com::Signals::SignalKeyType s_SLICE_INDEX_CHANGED_SIG = "sliceIndexChanged";
static const core::com::Signals::SignalKeyType s_PICKED_VOXEL_SIG        = "pickedVoxel";

//------------------------------------------------------------------------------

SNegato2D::SNegato2D() noexcept
{
    newSlot(s_SLICETYPE_SLOT, &SNegato2D::changeSliceType, this);
    newSlot(s_SLICEINDEX_SLOT, &SNegato2D::changeSliceIndex, this);
    newSlot(s_UPDATE_SLICES_FROM_WORLD, &SNegato2D::updateSlicesFromWorld, this);
    newSlot(s_UPDATE_TF_SLOT, &SNegato2D::updateTF, this);

    m_sliceIndexChangedSig = newSignal<SliceIndexChangedSignalType>(s_SLICE_INDEX_CHANGED_SIG);
    m_pickedVoxelSignal    = newSignal<PickedVoxelSigType>(s_PICKED_VOXEL_SIG);
}

//------------------------------------------------------------------------------

void SNegato2D::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration();

    static const std::string s_SLICE_INDEX_CONFIG = s_CONFIG + "sliceIndex";
    static const std::string s_FILTERING_CONFIG   = s_CONFIG + "filtering";
    static const std::string s_TF_ALPHA_CONFIG    = s_CONFIG + "tfAlpha";
    static const std::string s_BORDER_CONFIG      = s_CONFIG + "border";
    static const std::string s_INTERACTIVE_CONFIG = s_CONFIG + "interactive";

    const auto orientation = config.get<std::string>(s_SLICE_INDEX_CONFIG, "axial");
    if(orientation == "axial")
    {
        m_orientation = OrientationMode::Z_AXIS;
    }
    else if(orientation == "frontal")
    {
        m_orientation = OrientationMode::Y_AXIS;
    }
    else if(orientation == "sagittal")
    {
        m_orientation = OrientationMode::X_AXIS;
    }

    if(const auto filteringCfg = config.get_optional<std::string>(s_FILTERING_CONFIG); filteringCfg.has_value())
    {
        sight::viz::scene3d::Plane::filter_t filtering(sight::viz::scene3d::Plane::filter_t::LINEAR);

        if(filteringCfg.value() == "none")
        {
            filtering = sight::viz::scene3d::Plane::filter_t::NONE;
        }
        else if(filteringCfg.value() == "anisotropic")
        {
            filtering = sight::viz::scene3d::Plane::filter_t::ANISOTROPIC;
        }

        this->setFiltering(filtering);
    }

    m_enableAlpha = config.get<bool>(s_TF_ALPHA_CONFIG, m_enableAlpha);
    m_border      = config.get<bool>(s_BORDER_CONFIG, m_border);
    m_interactive = config.get<bool>(s_INTERACTIVE_CONFIG, m_interactive);

    const std::string transformId =
        config.get<std::string>(sight::viz::scene3d::ITransformable::s_TRANSFORM_CONFIG, this->getID() + "_transform");
    this->setTransformId(transformId);
}

//------------------------------------------------------------------------------

void SNegato2D::starting()
{
    this->initialize();
    this->getRenderService()->makeCurrent();
    {
        // 3D source texture instantiation
        const auto image = m_image.lock();
        m_3DOgreTexture = std::make_shared<sight::viz::scene3d::Texture>(image.get_shared());

        // TF texture initialization
        const auto tf = m_tf.lock();
        m_gpuTF = std::make_unique<sight::viz::scene3d::TransferFunction>(tf.get_shared());
    }

    // Scene node's instantiation
    Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    Ogre::SceneNode* const transformNode = this->getOrCreateTransformNode(rootSceneNode);
    m_negatoSceneNode = transformNode->createChildSceneNode();

    // Plane's instantiation
    m_plane = std::make_unique<sight::viz::scene3d::Plane>(
        this->getID(),
        m_negatoSceneNode,
        getSceneManager(),
        m_3DOgreTexture,
        m_filtering,
        m_border
    );

    this->newImage();
    this->setVisible(m_isVisible);

    if(m_interactive)
    {
        auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::IInteractor>(this->getSptr());
        this->getLayer()->addInteractor(interactor, 0);

        m_pickingCross = std::make_unique<sight::viz::scene3d::PickingCross>(
            this->getID(),
            *this->getSceneManager(),
            *m_negatoSceneNode
        );
    }
}

//------------------------------------------------------------------------------

void SNegato2D::stopping()
{
    this->getRenderService()->makeCurrent();

    m_pickingCross.reset();
    m_plane.reset();
    m_3DOgreTexture.reset();
    m_gpuTF.reset();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updating()
{
    this->newImage();
    this->setVisible(m_isVisible);
}

//------------------------------------------------------------------------------

void SNegato2D::newImage()
{
    if(!m_3DOgreTexture)
    {
        // The adaptor hasn't start yet (the window is maybe not visible)
        return;
    }

    this->getRenderService()->makeCurrent();

    int axialIdx    = 0;
    int frontalIdx  = 0;
    int sagittalIdx = 0;
    {
        const auto image = m_image.lock();

        if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
        {
            return;
        }

        // Retrieves or creates the slice index fields
        m_3DOgreTexture->update();

        const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());

        // Fits the plane to the new texture
        m_plane->update(m_orientation, spacing, origin, m_enableAlpha);

        // Update Slice
        namespace imHelper = data::helper::MedicalImage;

        axialIdx    = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::AXIAL).value_or(0)));
        frontalIdx  = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::FRONTAL).value_or(0)));
        sagittalIdx = std::max(0, int(imHelper::getSliceIndex(*image, imHelper::orientation_t::SAGITTAL).value_or(0)));
    }

    this->changeSliceIndex(axialIdx, frontalIdx, sagittalIdx);

    // Update transfer function in Gpu programs
    this->updateTF();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceType(int _from, int _to)
{
    const auto image = m_image.lock();

    const auto toOrientation   = static_cast<OrientationMode>(_to);
    const auto fromOrientation = static_cast<OrientationMode>(_from);

    const auto planeOrientation = m_orientation;
    const auto newOrientation   = m_orientation == toOrientation ? fromOrientation
                                                                 : planeOrientation
                                  == fromOrientation ? toOrientation : planeOrientation;

    if(planeOrientation != newOrientation)
    {
        m_orientation = newOrientation;
        this->getRenderService()->makeCurrent();

        const auto& [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
        m_plane->update(m_orientation, spacing, origin, m_enableAlpha);

        // Update threshold if necessary
        this->updateTF();

        this->updateShaderSliceIndexParameter();

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::changeSliceIndex(int _axialIndex, int _frontalIndex, int _sagittalIndex)
{
    const auto image = m_image.lock();

    this->getRenderService()->makeCurrent();

    auto imgSize = image->getSize();

    // Sometimes, the image can contain only one slice,
    // it results into a division by 0 when the range is transformed between [0-1].
    // So we increase the image size to 2 to divide by 1.
    imgSize[0] = imgSize[0] == 1 ? 2 : imgSize[0];
    imgSize[1] = imgSize[1] == 1 ? 2 : imgSize[1];
    imgSize[2] = imgSize[2] == 1 ? 2 : imgSize[2];

    m_currentSliceIndex = {
        static_cast<float>(_sagittalIndex) / (static_cast<float>(imgSize[0] - 1)),
        static_cast<float>(_frontalIndex) / (static_cast<float>(imgSize[1] - 1)),
        static_cast<float>(_axialIndex) / (static_cast<float>(imgSize[2] - 1))
    };

    this->updateShaderSliceIndexParameter();

    m_sliceIndexChangedSig->emit();

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::updateSlicesFromWorld(double _x, double _y, double _z)
{
    const auto image = m_image.lock();

    Ogre::Vector3 point = {static_cast<float>(_x), static_cast<float>(_y), static_cast<float>(_z)};
    Ogre::Vector3i slice_idx;
    try
    {
        slice_idx = sight::viz::scene3d::Utils::worldToSlices(*image, point);
    }
    catch(core::Exception& _e)
    {
        SIGHT_WARN("Cannot update slice index: " << _e.what());
        return;
    }

    const auto sig = image->signal<data::Image::SliceIndexModifiedSignalType>
                         (data::Image::s_SLICE_INDEX_MODIFIED_SIG);

    sig->asyncEmit(slice_idx[2], slice_idx[1], slice_idx[0]);
}

//------------------------------------------------------------------------------

void SNegato2D::updateShaderSliceIndexParameter()
{
    m_plane->changeSlice(m_currentSliceIndex[static_cast<std::size_t>(m_plane->getOrientationMode())]);
}

//------------------------------------------------------------------------------

void SNegato2D::updateTF()
{
    this->getRenderService()->makeCurrent();

    m_gpuTF->update();

    // Sends the TF texture to the negato-related passes
    m_plane->setTFData(*m_gpuTF);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SNegato2D::mouseMoveEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y, int /*_dx*/, int /*_dy*/)
{
    if(m_picked)
    {
        if(_button == MouseButton::LEFT)
        {
            this->pickIntensity(_x, _y);
        }

        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::buttonPressEvent(MouseButton _button, Modifier /*_mods*/, int _x, int _y)
{
    m_pickingCross->setVisible(false);
    if(_button == MouseButton::LEFT)
    {
        this->pickIntensity(_x, _y);
    }

    if(m_picked)
    {
        this->getLayer()->cancelFurtherInteraction();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::buttonReleaseEvent(MouseButton /*_button*/, Modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    m_picked = false;
    m_pickingCross->setVisible(false);
    m_pickedVoxelSignal->asyncEmit("");
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato2D::getAutoConnections() const
{
    return {
        {s_IMAGE_IN, data::Image::s_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_IMAGE_IN, data::Image::s_BUFFER_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_IMAGE_IN, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_SLICETYPE_SLOT},
        {s_IMAGE_IN, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_SLICEINDEX_SLOT},
        {s_TF_IN, data::TransferFunction::s_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_IN, data::TransferFunction::s_POINTS_MODIFIED_SIG, s_UPDATE_TF_SLOT},
        {s_TF_IN, data::TransferFunction::s_WINDOWING_MODIFIED_SIG, s_UPDATE_TF_SLOT}
    };
}

//------------------------------------------------------------------------------
void SNegato2D::setVisible(bool _visible)
{
    if(m_negatoSceneNode != nullptr)
    {
        m_negatoSceneNode->setVisible(_visible);

        this->requestRender();
    }
}

//------------------------------------------------------------------------------

void SNegato2D::pickIntensity(int _x, int _y)
{
    Ogre::SceneManager* sm = this->getSceneManager();

    const auto result = sight::viz::scene3d::Utils::pickObject(_x, _y, Ogre::SceneManager::ENTITY_TYPE_MASK, *sm);

    if(result.has_value())
    {
        if(m_plane->getMovableObject() == result->first)
        {
            m_picked = true;
            const auto image = m_image.lock();

            if(!data::helper::MedicalImage::checkImageValidity(image.get_shared()))
            {
                return;
            }

            const auto imageBufferLock = image->dump_lock();

            const auto [spacing, origin] = sight::viz::scene3d::Utils::convertSpacingAndOrigin(image.get_shared());
            auto crossLines = m_plane->computeCross(result->second, origin);
            m_pickingCross->update(crossLines[0], crossLines[1], crossLines[2], crossLines[3]);

            const auto pickingText = sight::viz::scene3d::Utils::pickImage(*image, result->second, origin, spacing);
            m_pickedVoxelSignal->asyncEmit(pickingText);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
