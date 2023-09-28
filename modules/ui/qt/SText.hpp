/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <ui/__/editor.hpp>

#include <QPointer>

class QLabel;

namespace sight::module::ui::qt
{

/**
 * @brief This service is used to display a simple label.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::SText">
        <label>my label</label>
        <color>#FF0000</color>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration
 * - \b label (optional, default="") : text to show before size of the vector
 * - \b color (optional, default="white") : needed color of the displayed label in a CSS style as names (ex: red),
 * rgb/rgba (ex: rgb(0,255,137,0.3)) or hexadecimal (ex: #355C66).
 */
class MODULE_UI_QT_CLASS_API SText final : public QObject,
                                           public sight::ui::editor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SText, sight::ui::editor);

    /// Initializes slots and member.
    MODULE_UI_QT_API SText();

    /// Destroys the service.
    MODULE_UI_QT_API ~SText() override = default;

private:

    /// Configures the service.
    void configuring() override;

    /// Installs the layout and gets the input data if it exists and displays it.
    void starting() override;

    /// Gets the input data if it exists and displays it.
    void updating() override;

    /// Destroys the layout.
    void stopping() override;

    /// Stores the static text to be displayed.
    QPointer<QLabel> m_label;
};

} // namespace sight::module::ui::qt
