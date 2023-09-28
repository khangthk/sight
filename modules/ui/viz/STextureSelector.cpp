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

#include "STextureSelector.hpp"

#include <core/com/signal.hxx>

#include <data/Array.hpp>
#include <data/Image.hpp>
#include <data/Material.hpp>

#include <io/__/service/ioTypes.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/dialog_editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------

STextureSelector::STextureSelector() noexcept =
    default;

//------------------------------------------------------------------------------

STextureSelector::~STextureSelector() noexcept =
    default;

//------------------------------------------------------------------------------

void STextureSelector::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_loadButton = new QPushButton(QString("Load texture"));
    m_loadButton->setObjectName(serviceID + "/" + m_loadButton->text());
    m_loadButton->setToolTip(QString("Selected organ's texture"));
    m_loadButton->setMinimumSize(m_loadButton->sizeHint());

    m_deleteButton = new QPushButton(QString("Remove texture"));
    m_deleteButton->setObjectName(serviceID + "/" + m_deleteButton->text());
    m_deleteButton->setToolTip(QString("Remove organ's texture"));
    m_deleteButton->setMinimumSize(m_deleteButton->sizeHint());

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_loadButton, 0);
    layout->addWidget(m_deleteButton, 0);

    qtContainer->setLayout(layout);
    qtContainer->setEnabled(true);

    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButton()));
    QObject::connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void STextureSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void STextureSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void STextureSelector::updating()
{
}

//------------------------------------------------------------------------------

void STextureSelector::onLoadButton()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    data::Image::sptr image       = material->getDiffuseTexture();

    bool existingTexture = (image != nullptr);

    // We have to instantiate a new image if necessary
    if(!existingTexture)
    {
        image = std::make_shared<data::Image>();
        material->setDiffuseTexture(image);
    }

    auto srv = sight::service::add<sight::ui::dialog_editor>("sight::module::ui::io::SSelector");
    srv->setInOut(image, io::service::s_DATA_KEY);

    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    sight::service::remove(srv);

    // If we didn't have to create a new texture, we can notify the associated image
    if(existingTexture)
    {
        auto sig = image->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
        sig->emit();
    }
    else
    {
        auto sig = material->signal<data::Material::AddedTextureSignalType>(
            data::Material::ADDED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

void STextureSelector::onDeleteButton()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    data::Image::sptr image       = material->getDiffuseTexture();

    if(image)
    {
        material->setDiffuseTexture(nullptr);
        auto sig = material->signal<data::Material::RemovedTextureSignalType>(
            data::Material::REMOVED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
