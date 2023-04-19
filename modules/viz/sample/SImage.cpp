/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/sample/SImage.hpp"

#include <service/op/Add.hpp>

#include <ui/base/GuiRegistry.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::module::viz::sample
{

static const std::string s_IMAGE_INPUT = "image";

//------------------------------------------------------------------------------

SImage::SImage() noexcept =
    default;

//------------------------------------------------------------------------------

SImage::~SImage() noexcept =
    default;

//------------------------------------------------------------------------------

void SImage::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void SImage::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    const auto genericSceneId = this->getID() + "-genericScene";
    sight::ui::base::GuiRegistry::registerSIDContainer(genericSceneId, qtContainer);

    // create and register the render service
    service::IService::ConfigType renderConfig;
    renderConfig.put("scene.background.<xmlattr>.color", "#36393E");
    renderConfig.put("scene.layer.<xmlattr>.id", "default");
    renderConfig.put("scene.layer.<xmlattr>.order", "1");
    renderConfig.put("scene.layer.<xmlattr>.transparency", "");

    service::IService::ConfigType interactorCfg;
    interactorCfg.put("<xmlattr>.uid", this->getID() + "interactorAdaptor");
    service::IService::ConfigType negatoCfg;
    negatoCfg.put("<xmlattr>.uid", this->getID() + "negato3DAdaptor");

    renderConfig.add_child("scene.layer.adaptor", interactorCfg);
    renderConfig.add_child("scene.layer.adaptor", negatoCfg);

    m_renderSrv = service::add("sight::viz::scene3d::SRender");
    m_renderSrv->setConfiguration(renderConfig);
    m_renderSrv->setID(genericSceneId);

    m_renderSrv->configure();

    m_interactorSrv = service::add("sight::module::viz::scene3d::adaptor::STrackballCamera");
    m_interactorSrv->setID(this->getID() + "interactorAdaptor");
    m_interactorSrv->configure();

    // Create default transfer function
    m_tf = data::TransferFunction::createDefaultTF();

    auto image = m_image.lock();
    service::IService::ConfigType negatoConfig;
    negatoConfig.put("config.<xmlattr>.interactive", "true");
    m_negatoSrv = service::add("sight::module::viz::scene3d::adaptor::SNegato3D");
    m_negatoSrv->setConfiguration(negatoConfig);
    m_negatoSrv->setInput(image.get_shared(), "image", true);
    m_negatoSrv->setInOut(m_tf, "tf", true);
    m_negatoSrv->setID(this->getID() + "negato3DAdaptor");
    m_negatoSrv->configure();

    m_renderSrv->start().wait();
    m_interactorSrv->start().wait();
    m_negatoSrv->start().wait();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SImage::getAutoConnections() const
{
    // This is actually useless since the sub-service already listens to the data,
    // but this prevents a warning in fwServices from being raised.
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void SImage::updating()
{
}

//------------------------------------------------------------------------------

void SImage::stopping()
{
    m_negatoSrv->stop().wait();
    m_interactorSrv->stop().wait();
    m_renderSrv->stop().wait();

    sight::ui::base::GuiRegistry::unregisterSIDContainer(this->getID() + "-genericScene");

    service::unregisterService(m_negatoSrv);
    service::unregisterService(m_interactorSrv);
    service::unregisterService(m_renderSrv);

    m_negatoSrv.reset();
    m_interactorSrv.reset();
    m_renderSrv.reset();
    m_tf.reset();

    this->destroy();
}

} // namespace sight::module::viz::sample.
