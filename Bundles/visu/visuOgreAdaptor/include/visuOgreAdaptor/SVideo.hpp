/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/TransferFunction.hpp>

#include <OGRE/OgreTexture.h>

namespace visuOgreAdaptor
{
/**
 * @brief Adaptor to render a video frame from a 2D-image.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether the video has to be seen or not.
 * - \b updateTF(): Updates the displayed transfer function.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SVideo" autoConnect="yes" >
            <in key="image" uid="..." />
            <in key="tf" uid="..." optional="yes" />
            <config layer="default" reversed="true" />
        </service>
   @endcode
 * @subsection Input Input:
 * - \b image [::fwData::Image]: frame displayed.
 * - \b tf [::fwData::TransferFunction] (optional): a transfer function that can be applied to the video.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the video's layer
 * - \b reverse (optional)(default: true) : if true, the actor is rotated by 180 degrees along the z and y axis.
 */
class VISUOGREADAPTOR_CLASS_API SVideo : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceMacro(SVideo, ::fwRenderOgre::IAdaptor)

    typedef ::fwCom::Signal< void ( double* ) > UpdateSizeSigType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_UPDATE_SIZE_SIG;

    /// Constructor.
    VISUOGREADAPTOR_API SVideo() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SVideo() noexcept;

    /**
     * @brief Sets whether the video is to be seen or not.
     * @param _isVisible Set to true to show the video.
     */
    VISUOGREADAPTOR_API void updateVisibility(bool _isVisible);

protected:

    /// Configure the options
    VISUOGREADAPTOR_API void configuring() override;

    /// Create the ogre texture and mapper used to show the video frame.
    VISUOGREADAPTOR_API void starting() override;

    /// Removes the actor from the renderer
    VISUOGREADAPTOR_API void stopping() override;

    /// Updated the frame from the current Image.
    VISUOGREADAPTOR_API void updating() override;

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: Updates the displayed transfer function
    void updateTF();

    /// Clean scene entities and associated mesh.
    void clearEntity();

    /// Ogre texture used to store the fwImage
    ::Ogre::TexturePtr m_texture;

    /// Ogre Material used to display the video plane
    ::Ogre::MaterialPtr m_material;

    /// Ogre Mesh used to display the video plane
    ::Ogre::MeshPtr m_mesh;

    /// Ogre Material used to display the video plane
    ::Ogre::SceneNode* m_sceneNode { nullptr };

    /// Ogre entity used to display the video plane
    ::Ogre::Entity* m_entity { nullptr };

    /// Contains and manages the Ogre textures used to store the transfer function (GPU point of view)
    std::unique_ptr< ::fwRenderOgre::TransferFunction> m_gpuTF;

    /// True if the texture is initialized
    bool m_isTextureInit { false };

    /// Store previous image size
    size_t m_previousWidth { 0 };

    /// Store previous image spacing
    size_t m_previousHeight { 0 };

    /// Store previous image type
    ::fwTools::Type m_previousType;

    /// If true, the actor is rotated in z and y axis.
    bool m_reverse { false };
};

} // visuOgreAdaptor
