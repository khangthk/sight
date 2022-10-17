/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/ui/qt/reconstruction/RepresentationEditor.hpp"

#include <core/com/Signal.hxx>
#include <core/runtime/path.hpp>

#include <data/Mesh.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

namespace sight::module::ui::qt::reconstruction
{

//------------------------------------------------------------------------------

RepresentationEditor::RepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

RepresentationEditor::~RepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void RepresentationEditor::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(getID().substr(getID().find_last_of('_') + 1));

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    auto* layout = new QVBoxLayout();

    auto* groupBox = new QGroupBox(tr("Representation"));
    groupBox->setObjectName(serviceID + "/" + groupBox->title());
    auto* layoutGroupBox = new QVBoxLayout();
    groupBox->setLayout(layoutGroupBox);

    m_buttonGroup = new QButtonGroup(groupBox);
    m_buttonGroup->setObjectName(serviceID + "/buttonGroup");

    auto* buttonSurface = new QRadioButton(tr("Surface"), groupBox);
    buttonSurface->setObjectName(serviceID + "/" + buttonSurface->text());
    buttonSurface->setMinimumSize(buttonSurface->sizeHint());
    m_buttonGroup->addButton(buttonSurface, 0);
    layoutGroupBox->addWidget(buttonSurface);
    buttonSurface->setChecked(true);

    auto* buttonPoint = new QRadioButton(tr("Point"), groupBox);
    buttonPoint->setObjectName(serviceID + "/" + buttonPoint->text());
    buttonPoint->setMinimumSize(buttonPoint->sizeHint());
    m_buttonGroup->addButton(buttonPoint, 1);
    layoutGroupBox->addWidget(buttonPoint);

    auto* buttonWireframe = new QRadioButton(tr("Wireframe"), groupBox);
    buttonWireframe->setObjectName(serviceID + "/" + buttonWireframe->text());
    buttonWireframe->setMinimumSize(buttonWireframe->sizeHint());
    m_buttonGroup->addButton(buttonWireframe, 2);
    layoutGroupBox->addWidget(buttonWireframe);

    auto* buttonEdge = new QRadioButton(tr("Edge"), groupBox);
    buttonEdge->setObjectName(serviceID + "/" + buttonEdge->text());
    buttonEdge->setMinimumSize(buttonEdge->sizeHint());
    m_buttonGroup->addButton(buttonEdge, 3);
    layoutGroupBox->addWidget(buttonEdge);

    // Shading group box
    auto* groupBoxShading = new QGroupBox(tr("Shading"));
    groupBoxShading->setObjectName(serviceID + "/" + groupBoxShading->title());
    auto* layoutGroupBoxShading = new QVBoxLayout();
    groupBoxShading->setLayout(layoutGroupBoxShading);
    m_buttonGroupShading = new QButtonGroup(groupBoxShading);
    m_buttonGroupShading->setObjectName(serviceID + "/buttonGroupShading");

    auto* buttonAmbient = new QRadioButton(tr("Ambient"), groupBoxShading);
    buttonAmbient->setObjectName(serviceID + "/" + buttonAmbient->text());
    buttonAmbient->setMinimumSize(buttonAmbient->sizeHint());
    m_buttonGroupShading->addButton(buttonAmbient, 0);
    layoutGroupBoxShading->addWidget(buttonAmbient);
    buttonAmbient->setChecked(true);

    auto* buttonFlat = new QRadioButton(tr("Flat"), groupBoxShading);
    buttonFlat->setObjectName(serviceID + "/" + buttonFlat->text());
    buttonFlat->setMinimumSize(buttonFlat->sizeHint());
    m_buttonGroupShading->addButton(buttonFlat, 1);
    layoutGroupBoxShading->addWidget(buttonFlat);
    buttonFlat->setChecked(true);

    auto* buttonPhong = new QRadioButton(tr("Phong"), groupBoxShading);
    buttonPhong->setObjectName(serviceID + "/" + buttonPhong->text());
    buttonPhong->setMinimumSize(buttonPhong->sizeHint());
    m_buttonGroupShading->addButton(buttonPhong, 2);
    layoutGroupBoxShading->addWidget(buttonPhong);

    layout->addWidget(groupBox);
    layout->addWidget(groupBoxShading);

    auto* groupBoxNormals = new QGroupBox(tr("Normals"));
    groupBoxNormals->setObjectName(serviceID + "/" + groupBoxNormals->title());
    auto* layoutGroupBoxNormals = new QVBoxLayout(groupBoxNormals);
    m_normalsRadioBox = new QButtonGroup();
    m_normalsRadioBox->setObjectName(serviceID + "/normalsRadioBox");
    auto* pointNormalsButton = new QRadioButton(tr("Show point normals"));
    pointNormalsButton->setObjectName(serviceID + "/" + pointNormalsButton->text());
    auto* cellNormalsButton = new QRadioButton(tr("Show cell normals"));
    cellNormalsButton->setObjectName(serviceID + "/" + cellNormalsButton->text());
    auto* hideNormalsButton = new QRadioButton(tr("Hide normals"));
    hideNormalsButton->setObjectName(serviceID + "/" + hideNormalsButton->text());

    m_normalsRadioBox->addButton(pointNormalsButton, 1);
    m_normalsRadioBox->addButton(cellNormalsButton, 2);
    m_normalsRadioBox->addButton(hideNormalsButton, 0);

    layoutGroupBoxNormals->addWidget(pointNormalsButton);
    layoutGroupBoxNormals->addWidget(cellNormalsButton);
    layoutGroupBoxNormals->addWidget(hideNormalsButton);

    layout->addWidget(groupBoxNormals);

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    qtContainer->setLayout(layout);
    qtContainer->setEnabled(false);

    QObject::connect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::connect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    this->updating();
}

//------------------------------------------------------------------------------

void RepresentationEditor::stopping()
{
    QObject::disconnect(m_buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onChangeRepresentation(int)));
    QObject::disconnect(m_buttonGroupShading, SIGNAL(buttonClicked(int)), this, SLOT(onChangeShading(int)));

    QObject::connect(m_normalsRadioBox, SIGNAL(buttonClicked(int)), this, SLOT(onShowNormals(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void RepresentationEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void RepresentationEditor::updating()
{
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer()
    );
    QWidget* const container = qtContainer->getQtContainer();
    SIGHT_ASSERT("container not instanced", container);

    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();
    m_material = reconstruction->getMaterial();
    container->setEnabled(!reconstruction->getOrganName().empty());

    this->refreshRepresentation();
    this->refreshNormals();
    this->refreshShading();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeRepresentation(int id)
{
    data::Material::RepresentationType selectedMode = data::Material::SURFACE;

    switch(id)
    {
        case 1:
            selectedMode = data::Material::POINT;
            break;

        case 2:
            selectedMode = data::Material::WIREFRAME;
            break;

        case 3:
            selectedMode = data::Material::EDGE;
            break;

        default: // 0 or other
            selectedMode = data::Material::SURFACE;
    }

    m_material->setRepresentationMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::onChangeShading(int id)
{
    data::Material::ShadingType selectedMode = data::Material::PHONG;

    switch(id)
    {
        case 0:
            selectedMode = data::Material::AMBIENT;
            break;

        case 1:
            selectedMode = data::Material::FLAT;
            break;

        default: // 2 or other
            selectedMode = data::Material::PHONG;
    }

    m_material->setShadingMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshRepresentation()
{
    int representationMode  = m_material->getRepresentationMode();
    QAbstractButton* button = nullptr;

    switch(representationMode)
    {
        case data::Material::SURFACE:
            button = m_buttonGroup->button(0);
            button->setChecked(true);
            break;

        case data::Material::POINT:
            button = m_buttonGroup->button(1);
            button->setChecked(true);
            break;

        case data::Material::WIREFRAME:
            button = m_buttonGroup->button(2);
            button->setChecked(true);
            break;

        case data::Material::EDGE:
            button = m_buttonGroup->button(3);
            button->setChecked(true);
            break;

        default:
            button = m_buttonGroup->button(0);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshShading()
{
    int shadingMode         = m_material->getShadingMode();
    QAbstractButton* button = nullptr;

    switch(shadingMode)
    {
        case data::Material::AMBIENT:
            button = m_buttonGroupShading->button(0);
            button->setChecked(true);
            break;

        case data::Material::FLAT:
            button = m_buttonGroupShading->button(1);
            button->setChecked(true);
            break;

        case data::Material::PHONG:
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
            break;

        default:
            button = m_buttonGroupShading->button(2);
            button->setChecked(true);
    }
}

//------------------------------------------------------------------------------

void RepresentationEditor::refreshNormals()
{
    QAbstractButton* buttonHide = m_normalsRadioBox->button(0);
    buttonHide->setChecked(m_material->getOptionsMode() == data::Material::STANDARD);
    QAbstractButton* buttonNormals = m_normalsRadioBox->button(1);
    buttonNormals->setChecked(m_material->getOptionsMode() == data::Material::NORMALS);
}

//------------------------------------------------------------------------------

void RepresentationEditor::onShowNormals(int state)
{
    switch(state)
    {
        case 0:
            m_material->setOptionsMode(data::Material::STANDARD);
            break;

        case 1:
            m_material->setOptionsMode(data::Material::NORMALS);
            break;

        case 2:
            m_material->setOptionsMode(data::Material::CELLS_NORMALS);
            break;

        default:
            SIGHT_ASSERT("Invalid state: " << state, false);
    }

    this->notifyMaterial();

    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal<data::Reconstruction::MeshChangedSignalType>(
        data::Reconstruction::s_MESH_CHANGED_SIG
    );
    sig->asyncEmit(reconstruction->getMesh());
}

//------------------------------------------------------------------------------

void RepresentationEditor::notifyMaterial()
{
    SIGHT_ASSERT("The inout key '" << s_RECONSTRUCTION << "' is not defined.", !m_rec.expired());
    auto reconstruction = m_rec.lock();

    data::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::Object::ModifiedSignalType>(
        data::Object::s_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap RepresentationEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::reconstruction
