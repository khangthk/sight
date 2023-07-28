/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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

// cspell:ignore NOLINT

#include "viz/scene3d/vr/IVolumeRenderer.hpp"

#include "viz/scene3d/Layer.hpp"

#include <boost/algorithm/clamp.hpp>

#include <utility>

namespace sight::viz::scene3d::vr
{

const std::array<Ogre::Vector3, 8> IVolumeRenderer::s_imagePositions =
{
    Ogre::Vector3(1, 1, 1),
    Ogre::Vector3(1, 0, 1),
    Ogre::Vector3(1, 1, 0),
    Ogre::Vector3(0, 1, 1),
    Ogre::Vector3(0, 0, 1),
    Ogre::Vector3(1, 0, 0),
    Ogre::Vector3(0, 1, 0),
    Ogre::Vector3(0, 0, 0)
};

//-----------------------------------------------------------------------------

IVolumeRenderer::IVolumeRenderer(
    std::string parentId,
    Ogre::SceneManager* const sceneManager,
    Ogre::SceneNode* const volumeNode,
    sight::data::Image::csptr image,
    sight::data::Image::csptr mask,
    sight::data::TransferFunction::csptr tf,
    std::uint16_t samples,
    bool with_buffer,
    bool preintegration
) :
    m_parentId(std::move(parentId)),
    m_sceneManager(sceneManager),
    m_3DOgreTexture(std::make_shared<sight::viz::scene3d::Texture>(image)),
    m_maskTexture(std::make_shared<sight::viz::scene3d::Texture>(mask)),
    m_gpuVolumeTF(std::make_shared<sight::viz::scene3d::TransferFunction>(tf)),
    m_with_buffer(with_buffer),
    m_preintegration(preintegration),
    m_volumeSceneNode(volumeNode),
    m_camera(m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME)),
    m_nbSlices(samples),
    m_clippedImagePositions(s_imagePositions)
{
    //Transfer function and preintegration table
    {
        m_preIntegrationTable.createTexture(m_parentId);
    }

    // 3D source texture instantiation

    if(m_with_buffer)
    {
        // 3D source texture instantiation
        m_bufferingTexture = std::make_shared<sight::viz::scene3d::Texture>(image, "_buffered");
    }
}

//-----------------------------------------------------------------------------

IVolumeRenderer::~IVolumeRenderer()
{
    m_maskTexture.reset();
    m_3DOgreTexture.reset();

    if(m_bufferingTexture)
    {
        m_bufferingTexture.reset();
    }

    m_preIntegrationTable.removeTexture();
}

//------------------------------------------------------------------------------

void IVolumeRenderer::loadImage()
{
    if(m_with_buffer)
    {
        m_bufferingTexture->update();

        // Swap texture pointers.
        {
            std::lock_guard<std::mutex> swapLock(m_bufferSwapMutex);
            std::swap(m_3DOgreTexture, m_bufferingTexture);
        }
    }
    else
    {
        m_3DOgreTexture->update();
    }
}

//------------------------------------------------------------------------------

void IVolumeRenderer::loadMask()
{
    m_maskTexture->update();
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::clipImage(const Ogre::AxisAlignedBox& clippingBox)
{
    const Ogre::Vector3 min = clippingBox.getMinimum();
    const Ogre::Vector3 max = clippingBox.getMaximum();

    for(unsigned i = 0 ; i < 8 ; ++i)
    {
        m_clippedImagePositions[i] = Ogre::Vector3(
            boost::algorithm::clamp(s_imagePositions[i].x, min.x, max.x),
            boost::algorithm::clamp(s_imagePositions[i].y, min.y, max.y),
            boost::algorithm::clamp(s_imagePositions[i].z, min.z, max.z)
        );
    }
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::resizeViewport(int /*w*/, int /*h*/)
{
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::scaleTranslateCube(
    const data::Image::Spacing& spacing,
    const data::Image::Origin& origin
)
{
    // Scale the volume based on the image's spacing and move it to the image origin.
    m_volumeSceneNode->resetToInitialState();

    const double width  = static_cast<double>(m_3DOgreTexture->width()) * spacing[0];
    const double height = static_cast<double>(m_3DOgreTexture->height()) * spacing[1];
    const double depth  = static_cast<double>(m_3DOgreTexture->depth()) * spacing[2];

    const Ogre::Vector3 scaleFactors(
        static_cast<float>(width),
        static_cast<float>(height),
        static_cast<float>(depth));

    const Ogre::Vector3 ogreOrigin(
        static_cast<float>(origin[0]),
        static_cast<float>(origin[1]),
        static_cast<float>(origin[2]));

    m_volumeSceneNode->setScale(scaleFactors);
    m_volumeSceneNode->setPosition(ogreOrigin);
}

//-----------------------------------------------------------------------------

void IVolumeRenderer::updateSampleDistance()
{
    //Update the plane with the current position and direction
    m_cameraInfo.plane = Ogre::Plane(
        m_volumeSceneNode->convertWorldToLocalDirection(m_camera->getRealDirection(), true).normalisedCopy(),
        m_volumeSceneNode->convertWorldToLocalPosition(m_camera->getRealPosition())
    );

    //Compares the distances to the camera plane to get the cube's closest and furthest vertex to the camera
    const auto comp = [this](const Ogre::Vector3& v1, const Ogre::Vector3& v2)
                      {return m_cameraInfo.plane.getDistance(v1) < m_cameraInfo.plane.getDistance(v2);};

    //Closest vertex
    {
        const auto iterator = std::min_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));
        m_cameraInfo.closest       = *iterator;
        m_cameraInfo.closest_index = index;
    }

    //Furthest vertex
    {
        const auto iterator = std::max_element( // NOLINT(readability-qualified-auto,llvm-qualified-auto)
            m_clippedImagePositions.begin(),
            m_clippedImagePositions.end(),
            comp
        );
        const auto index = static_cast<std::size_t>(std::distance(m_clippedImagePositions.begin(), iterator));

        m_cameraInfo.furthest       = *iterator;
        m_cameraInfo.furthest_index = index;
    }

    //The total distance between the vertices
    const float total_distance =
        std::abs(
            m_cameraInfo.plane.getDistance(m_cameraInfo.closest)
            - m_cameraInfo.plane.getDistance(m_cameraInfo.furthest)
        );

    //Then simply uniformly divide it according to the total number of slices
    m_sampleDistance = total_distance / static_cast<float>(m_nbSlices);

    //Validation
    SIGHT_ASSERT("Sampled distance is NaN.", !std::isnan(m_sampleDistance)); //NaN
    SIGHT_ASSERT("Sample distance is denormalized.", m_sampleDistance > std::numeric_limits<float>::min());
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
