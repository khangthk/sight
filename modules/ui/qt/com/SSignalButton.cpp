/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include "SSignalButton.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>

#include <ui/qt/container/widget.hpp>

#include <QVariant>
#include <QVBoxLayout>

namespace sight::module::ui::qt::com
{

static const core::com::signals::key_t CLICKED_SIG = "clicked";
static const core::com::signals::key_t TOGGLED_SIG = "toggled";

static const core::com::slots::key_t SET_CHECKED_SLOT = "setChecked";
static const core::com::slots::key_t CHECK_SLOT       = "check";
static const core::com::slots::key_t UNCHECK_SLOT     = "uncheck";

static const core::com::slots::key_t SET_ENABLED_SLOT = "setEnabled";
static const core::com::slots::key_t ENABLE_SLOT      = "enable";
static const core::com::slots::key_t DISABLE_SLOT     = "disable";
static const core::com::slots::key_t SET_VISIBLE_SLOT = "setVisible";
static const core::com::slots::key_t SHOW_SLOT        = "show";
static const core::com::slots::key_t HIDE_SLOT        = "hide";

//-----------------------------------------------------------------------------

SSignalButton::SSignalButton() noexcept :
    m_sigClicked(new_signal<ClickedSignalType>(CLICKED_SIG)),
    m_sigToggled(new_signal<ToggledSignalType>(TOGGLED_SIG))
{
    new_slot(SET_CHECKED_SLOT, &SSignalButton::setChecked, this);
    new_slot(CHECK_SLOT, &SSignalButton::check, this);
    new_slot(UNCHECK_SLOT, &SSignalButton::uncheck, this);
    new_slot(SET_ENABLED_SLOT, &SSignalButton::setEnabled, this);
    new_slot(ENABLE_SLOT, &SSignalButton::enable, this);
    new_slot(DISABLE_SLOT, &SSignalButton::disable, this);
    new_slot(SET_VISIBLE_SLOT, &SSignalButton::setVisible, this);
    new_slot(SHOW_SLOT, &SSignalButton::show, this);
    new_slot(HIDE_SLOT, &SSignalButton::hide, this);
}

//-----------------------------------------------------------------------------

SSignalButton::~SSignalButton() noexcept =
    default;

//-----------------------------------------------------------------------------

void SSignalButton::configuring()
{
    this->initialize();

    const auto configuration = this->getConfiguration();

    const auto config = configuration.get_child_optional("config");

    if(config.has_value())
    {
        m_checkable    = config->get<bool>("checkable", m_checkable);
        m_checkAtStart = config->get<bool>("checked", m_checkAtStart);
        m_enable       = config->get<bool>("enable", m_enable);

        m_text    = config->get<std::string>("text", m_text);
        m_text2   = config->get<std::string>("text2", m_text2);
        m_toolTip = config->get<std::string>("toolTip", m_toolTip);

        if(const auto icon = config->get_optional<std::string>("icon"); icon.has_value())
        {
            m_icon = core::runtime::get_module_resource_file_path(icon.value());
        }

        if(const auto icon = config->get_optional<std::string>("icon2"); icon.has_value())
        {
            m_icon2 = core::runtime::get_module_resource_file_path(icon.value());
        }

        m_iconWidth  = config->get<unsigned int>("iconWidth", m_iconWidth);
        m_iconHeight = config->get<unsigned int>("iconHeight", m_iconHeight);
    }
}

//-----------------------------------------------------------------------------

void SSignalButton::starting()
{
    this->create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* layout = new QVBoxLayout();
    m_button = new QPushButton(QString::fromStdString(m_text));
    m_button->setEnabled(m_enable);
    m_button->setProperty("class", "signal-button");
    layout->addWidget(m_button);
    qtContainer->setLayout(layout);

    if(!m_toolTip.empty())
    {
        m_button->setToolTip(QString::fromStdString(m_toolTip));
    }

    if(!m_icon.empty())
    {
        m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
    }

    if(m_iconWidth > 0 && m_iconHeight > 0)
    {
        m_button->setIconSize(QSize(int(m_iconWidth), int(m_iconHeight)));
    }

    if(m_checkable)
    {
        m_button->setCheckable(true);

        if(m_checkAtStart)
        {
            m_button->setChecked(true);
            if(!m_text2.empty())
            {
                m_button->setText(QString::fromStdString(m_text2));
            }

            if(!m_icon2.empty())
            {
                m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
            }
        }
    }

    QObject::connect(m_button.data(), &QPushButton::clicked, this, &SSignalButton::onClicked);
    QObject::connect(m_button.data(), &QPushButton::toggled, this, &SSignalButton::onToggled);
}

//-----------------------------------------------------------------------------

void SSignalButton::updating()
{
}

//-----------------------------------------------------------------------------

void SSignalButton::stopping()
{
    this->destroy();
}

//-----------------------------------------------------------------------------

void SSignalButton::onClicked()
{
    m_sigClicked->async_emit();
}

//-----------------------------------------------------------------------------

void SSignalButton::onToggled(bool toggled)
{
    this->setChecked(toggled);
    m_sigToggled->async_emit(toggled);
}

//-----------------------------------------------------------------------------

void SSignalButton::setChecked(bool checked)
{
    if(checked)
    {
        if(!m_text2.empty())
        {
            m_button->setText(QString::fromStdString(m_text2));
        }

        if(!m_icon2.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon2.string())));
        }
    }
    else
    {
        if(!m_text.empty())
        {
            m_button->setText(QString::fromStdString(m_text));
        }

        if(!m_icon.empty())
        {
            m_button->setIcon(QIcon(QString::fromStdString(m_icon.string())));
        }
    }

    // properly check/uncheck the button when this method is called from the sight slot.
    this->blockSignals(true);
    m_button->setChecked(checked);
    this->blockSignals(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::check()
{
    this->setChecked(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::uncheck()
{
    this->setChecked(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::setEnabled(bool _isEnabled)
{
    editor::setEnabled(_isEnabled);
    // Keep this in case of SSignalButton is used outside a view container
    m_button->setEnabled(_isEnabled);
}

//-----------------------------------------------------------------------------

void SSignalButton::enable()
{
    this->setEnabled(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::disable()
{
    this->setEnabled(false);
}

//-----------------------------------------------------------------------------

void SSignalButton::setVisible(bool _isVisible)
{
    editor::setVisible(_isVisible);
    // Keep this in case of SSignalButton is used outside a view container
    m_button->setVisible(_isVisible);
}

//-----------------------------------------------------------------------------

void SSignalButton::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void SSignalButton::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::com
