/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "ui/dicom/splitter/tag_value_instance_remove_configurable_splitter.hpp"

#include "ui/dicom/widget/qtag_selector_widget.hpp"

#include <filter/dicom/registry/macros.hpp>

#include <dcmtk/dcmdata/dcdeftag.h>

#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

SIGHT_REGISTER_DICOM_FILTER(sight::ui::dicom::splitter::tag_value_instance_remove_configurable_splitter);

namespace sight::ui::dicom::splitter
{

const std::string tag_value_instance_remove_configurable_splitter::FILTER_NAME =
    "Tag value instance remove configurable splitter";
const std::string tag_value_instance_remove_configurable_splitter::FILTER_DESCRIPTION =
    "Remove instances if a tag has a certain value.";

//-----------------------------------------------------------------------------

std::string tag_value_instance_remove_configurable_splitter::get_name() const
{
    return tag_value_instance_remove_configurable_splitter::FILTER_NAME;
}

//-----------------------------------------------------------------------------

std::string tag_value_instance_remove_configurable_splitter::get_description() const
{
    return tag_value_instance_remove_configurable_splitter::FILTER_DESCRIPTION;
}

//-----------------------------------------------------------------------------

bool tag_value_instance_remove_configurable_splitter::is_configuration_required() const
{
    return true;
}

//-----------------------------------------------------------------------------

bool tag_value_instance_remove_configurable_splitter::is_configurable_with_gui() const
{
    return true;
}

//-----------------------------------------------------------------------------

void tag_value_instance_remove_configurable_splitter::configure_with_gui()
{
    auto* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Configure"));
    auto* main_layout = new QVBoxLayout();
    dialog->setLayout(main_layout);
    dialog->setMinimumWidth(500);

    // Create tag selectors
    auto* tag_selector =
        new ui::dicom::widget::q_tag_selector_widget();
    tag_selector->set_tag_value(m_tag);
    main_layout->addWidget(tag_selector);

    // Create tag value field
    auto* tag_value_widget = new QWidget();
    main_layout->addWidget(tag_value_widget);
    auto* tag_value_layout = new QHBoxLayout();
    tag_value_widget->setLayout(tag_value_layout);
    tag_value_layout->addWidget(new QLabel("<b>Value:</b> "));
    auto* tag_valueline_edit = new QLineEdit(m_tag_value.c_str());
    tag_value_layout->addWidget(tag_valueline_edit, 1);
    tag_value_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Create buttons
    auto* button_box = new QDialogButtonBox(dialog);
    main_layout->addWidget(button_box);
    QPushButton* ok_button     = button_box->addButton(QDialogButtonBox::Ok);
    QPushButton* cancel_button = button_box->addButton(QDialogButtonBox::Cancel);

    QObject::connect(ok_button, SIGNAL(clicked(void)), dialog, SLOT(accept(void)));
    QObject::connect(cancel_button, SIGNAL(clicked(void)), dialog, SLOT(reject(void)));

    int result = dialog->exec();
    if(result == QDialog::Accepted)
    {
        m_tag       = tag_selector->get_tag();
        m_tag_value = tag_valueline_edit->text().toStdString();
    }
}

} // namespace sight::ui::dicom::splitter
