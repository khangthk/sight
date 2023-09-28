/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "SSelectionMenuButton.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <ui/qt/container/widget.hpp>

#include <boost/range/iterator_range_core.hpp>

#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

#include <filesystem>

namespace sight::module::ui::qt
{

static const core::com::signals::key_t SELECTED_SIG = "selected";

static const core::com::slots::key_t SET_ENABLED_SIG = "setEnabled";
static const core::com::slots::key_t SENABLE_SIG     = "enable";
static const core::com::slots::key_t DISABLE_SIG     = "disable";

SSelectionMenuButton::SSelectionMenuButton() noexcept :
    m_sigSelected(new_signal<SelectedSignalType>(SELECTED_SIG))
{
    new_slot(SET_ENABLED_SIG, &SSelectionMenuButton::setEnabled, this);
    new_slot(SENABLE_SIG, &SSelectionMenuButton::enable, this);
    new_slot(DISABLE_SIG, &SSelectionMenuButton::disable, this);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::configuring()
{
    this->initialize();

    const auto& config = this->getConfiguration();

    m_text      = config.get<std::string>("text", m_text);
    m_toolTip   = config.get<std::string>("toolTip", m_toolTip);
    m_selection = config.get<int>("selected", m_selection);

    const auto& items = config.get_child("items");
    for(const auto& elem : boost::make_iterator_range(items.equal_range("item")))
    {
        const auto txt  = elem.second.get<std::string>("<xmlattr>.text");
        const int value = elem.second.get<int>("<xmlattr>.value");
        m_items.emplace_back(value, txt);
    }
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    m_dropDownButton = new QPushButton(QString::fromStdString(m_text));
    m_dropDownButton->setToolTip(QString::fromStdString(m_toolTip));
//    m_dropDownButton->setMaximumWidth(40);

    m_pDropDownMenu = new QMenu();
    m_actionGroup   = new QActionGroup(m_pDropDownMenu);

    for(const auto& item : m_items)
    {
        auto* action = new QAction(QString::fromStdString(item.second), m_pDropDownMenu);
        action->setCheckable(true);
        action->setData(QVariant(item.first));
        m_actionGroup->addAction(action);
        m_pDropDownMenu->addAction(action);

        if(item.first == m_selection)
        {
            action->setChecked(true);
        }
    }

    QObject::connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onSelection(QAction*)));
    m_dropDownButton->setMenu(m_pDropDownMenu);

    auto* vLayout = new QVBoxLayout();
    vLayout->addWidget(m_dropDownButton);
    vLayout->setContentsMargins(0, 0, 0, 0);

    qtContainer->setLayout(vLayout);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::stopping()
{
    QObject::connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onSelection(QAction*)));
    for(QAction* action : m_actionGroup->actions())
    {
        m_actionGroup->removeAction(action);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::updating()
{
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::info(std::ostream& /*_sstream*/)
{
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::onSelection(QAction* action)
{
    if(action->isChecked())
    {
        int value = action->data().toInt();
        m_sigSelected->async_emit(value);
        return;
    }
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::setEnabled(bool enabled)
{
    m_dropDownButton->setEnabled(enabled);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::enable()
{
    this->setEnabled(true);
}

//------------------------------------------------------------------------------

void SSelectionMenuButton::disable()
{
    this->setEnabled(false);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
