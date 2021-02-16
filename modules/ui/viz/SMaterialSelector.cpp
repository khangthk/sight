/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "SMaterialSelector.hpp"

#include <core/com/Signal.hxx>

#include <data/Material.hpp>
#include <data/String.hpp>
#include <data/tools/helper/Field.hpp>

#include <service/macros.hpp>

#include <viz/ogre/ogre.hpp>
#include <viz/ogre/Utils.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreTechnique.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <ui/qt/container/QtContainer.hpp>

namespace sight::modules::ui::viz
{

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::modules::ui::viz::SMaterialSelector,
                         ::sight::data::Reconstruction)

const core::com::Signals::SignalKeyType SMaterialSelector::s_SELECTED_SIG = "selected";

static const service::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

static const std::string s_MATERIAL_RESOURCEGROUP_NAME = "materialsTemplate";

//------------------------------------------------------------------------------
SMaterialSelector::SMaterialSelector() noexcept
{
    newSignal< SelectedSignalType >( s_SELECTED_SIG );
}

//------------------------------------------------------------------------------

SMaterialSelector::~SMaterialSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SMaterialSelector::starting()
{
    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer() );

    // Selection
    QLabel* currentMaterial = new QLabel();
    currentMaterial->setText("Current material : ");

    m_materialBox = new QComboBox();

    std::pair <std::string, std::string> elt;
    ::Ogre::ResourceManager::ResourceMapIterator iter = ::Ogre::MaterialManager::getSingleton().getResourceIterator();
    while(iter.hasMoreElements())
    {
        ::Ogre::ResourcePtr mat = iter.getNext();
        if (mat->getGroup() == s_MATERIAL_RESOURCEGROUP_NAME)
        {
            m_materialBox->addItem(QString::fromStdString(mat->getName()));
        }
    }
    m_materialBox->setCurrentIndex(0);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->addWidget( currentMaterial);
    labelLayout->addWidget( m_materialBox);

    // Reload
    m_reloadButton = new QPushButton("Reload");

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout( labelLayout );
    layout->addWidget( m_reloadButton );

    qtContainer->setLayout( layout );

    this->updating();

    QObject::connect(m_materialBox, SIGNAL(activated(const QString&)), this,
                     SLOT(onSelectedModeItem(const QString&)));
    QObject::connect(m_reloadButton, SIGNAL(clicked()), this, SLOT(onReloadMaterial()));
}

//------------------------------------------------------------------------------

void SMaterialSelector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SMaterialSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SMaterialSelector::updating()
{
    this->updateMaterial();
}

//------------------------------------------------------------------------------

void SMaterialSelector::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SMaterialSelector::updateMaterial()
{
    data::Reconstruction::sptr reconst = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    data::Material::sptr material      = reconst->getMaterial();
    data::Object::sptr fieldObj        = material->getField("ogreMaterial");
    if(fieldObj != nullptr)
    {
        data::String::sptr field = data::String::dynamicCast(fieldObj);
        m_materialBox->setCurrentText(field->value().c_str());
    }
}

//------------------------------------------------------------------------------

void SMaterialSelector::onSelectedModeItem(const QString& text)
{
    data::Reconstruction::sptr reconst = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    data::Material::sptr material      = reconst->getMaterial();
    data::String::sptr string          = data::String::New();
    string->setValue(text.toStdString());

    data::tools::helper::Field helper(material);
    helper.setField("ogreMaterial", string);
    helper.notify();

    auto sig = this->signal<SelectedSignalType>(s_SELECTED_SIG);
    sig->asyncEmit(text.toStdString());
}

//------------------------------------------------------------------------------

void SMaterialSelector::onReloadMaterial()
{
    auto materialName = m_materialBox->currentText().toStdString();
    ::Ogre::MaterialPtr material = ::Ogre::MaterialManager::getSingleton().getByName(materialName,
                                                                                     sight::viz::ogre::RESOURCE_GROUP);

    if(!material)
    {
        SLM_ERROR("Could not find material" << materialName);
    }

    material->reload();

    const ::Ogre::Material::Techniques& techniques = material->getTechniques();

    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        const ::Ogre::Technique::Passes& passes = tech->getPasses();

        for(const auto pass : passes)
        {
            SLM_ASSERT("No pass found", pass);

            if(!pass->getVertexProgramName().empty())
            {
                pass->getVertexProgram()->reload();
            }
            if(!pass->getGeometryProgramName().empty())
            {
                pass->getGeometryProgram()->reload();
            }
            if(!pass->getFragmentProgramName().empty())
            {
                pass->getFragmentProgram()->reload();
            }
        }

    }

}

//------------------------------------------------------------------------------

} // namespace sight::modules::ui::viz
