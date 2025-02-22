/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/text.hpp"

#include <core/com/slots.hxx>

#include <data/string_serializable.hpp>

#include <viz/scene3d/render.hpp>
#include <viz/scene3d/window_interactor.hpp>

#include <regex>

// cspell:ignore RRGGBB RRGGBBAA
namespace sight::module::viz::scene3d::adaptor
{

static const core::com::slots::key_t SET_TEXT_SLOT  = "set_text";
static const core::com::slots::key_t SET_COLOR_SLOT = "set_color";

// Helper function to control color format in hexa.
bool check_color_format(const std::string& _color)
{
    // Check that _color matches #RRGGBB or #RRGGBBAA in hexa format.
    const std::regex pattern("#[0-9a-fA-F]{6}(?:[0-9a-fA-F]{2})?");
    std::smatch match;
    return std::regex_match(_color, match, pattern);
}

//----------------------------------------------------------------------------

text::text() noexcept
{
    new_slot(SET_TEXT_SLOT, &text::set_text, this);
    new_slot(SET_COLOR_SLOT, &text::set_color, this);
}

//----------------------------------------------------------------------------

void text::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_TEXT_CONFIG        = "text";
    static const std::string s_FONT_SIZE_CONFIG   = CONFIG + "fontSize";
    static const std::string s_FONT_SOURCE_CONFIG = CONFIG + "fontSource";
    static const std::string s_H_ALIGN_CONFIG     = CONFIG + "hAlign";
    static const std::string s_V_ALIGN_CONFIG     = CONFIG + "vAlign";
    static const std::string s_X_CONFIG           = CONFIG + "x";
    static const std::string s_Y_CONFIG           = CONFIG + "y";
    static const std::string s_COLOR_CONFIG       = CONFIG + "color";

    m_text_string = config.get<std::string>(s_TEXT_CONFIG, "");

    m_font_source = config.get(s_FONT_SOURCE_CONFIG, m_font_source);
    m_font_size   = config.get<std::size_t>(s_FONT_SIZE_CONFIG, m_font_size);

    m_horizontal_alignment = config.get<std::string>(s_H_ALIGN_CONFIG, "left");
    SIGHT_ASSERT(
        "'hAlign' must be 'left', 'center' or 'right'",
        m_horizontal_alignment == "left"
        || m_horizontal_alignment == "center"
        || m_horizontal_alignment == "right"
    );

    m_vertical_alignment = config.get<std::string>(s_V_ALIGN_CONFIG, "bottom");
    SIGHT_ASSERT(
        "'vAlign' must be 'top', 'center' or 'bottom'",
        m_vertical_alignment == "top"
        || m_vertical_alignment == "center"
        || m_vertical_alignment == "bottom"
    );

    m_position.x = config.get<float>(s_X_CONFIG, m_position.x);
    m_position.y = config.get<float>(s_Y_CONFIG, m_position.y);

    const auto color = config.get<std::string>(s_COLOR_CONFIG, "#FFFFFF");

    if(check_color_format(color))
    {
        m_text_color = color;
    }
    else
    {
        SIGHT_WARN("Color '" + color + "' isn't a valid color format (#RRGGBB or #RRGGBBAA in hexa)");
    }
}

//----------------------------------------------------------------------------

void text::starting()
{
    this->initialize();

    auto render_srv = this->render_service();

    m_text = sight::viz::scene3d::text::make(this->layer());

    m_text->set_font_size(m_font_size);
    m_text->set_text_color(m_text_color);

    this->update_text();
}

//-----------------------------------------------------------------------------

service::connections_t text::auto_connections() const
{
    connections_t connections = adaptor::auto_connections();
    connections.push(OBJECT_INPUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//----------------------------------------------------------------------------

void text::updating()
{
    this->render_service()->make_current();
    this->update_text();
}

//----------------------------------------------------------------------------

void text::stopping()
{
    m_text = nullptr;
}

//----------------------------------------------------------------------------

void text::set_text(std::string _str)
{
    m_text_string = _str;
    m_text->set_text(_str);
    m_text->set_text_color(m_text_color);
    m_text->set_text_alignment(m_horizontal_alignment, m_vertical_alignment);
}

//----------------------------------------------------------------------------

void text::set_color(std::string _color)
{
    if(check_color_format(_color))
    {
        m_text->set_text_color(_color);
    }
    else
    {
        SIGHT_WARN("Color '" + _color + "' isn't a valid color format (#RRGGBB or #RRGGBBAA in hexa)");
    }
}

//----------------------------------------------------------------------------

void text::update_text()
{
    std::string text_string = m_text_string;

    const auto obj = m_object.lock();

    if(obj)
    {
        const auto field = std::dynamic_pointer_cast<const data::string_serializable>(obj.get_shared());

        if(field)
        {
            text_string = field->to_string();
        }
    }

    this->set_text(text_string);
}

} // namespace sight::module::viz::scene3d::adaptor.
