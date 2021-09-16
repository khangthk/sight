/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include <data/Material.hpp>

#include <ui/base/IEditor.hpp>

#include <QObject>
#include <QPointer>

class QSlider;
class QLabel;

namespace sight::module::ui::qt
{

/**
 * @brief Adds an opacity editor widget (Slider) applied on a data::Material.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::ui::qt::SMaterialOpacityEditor">
       <inout key="material" uid="..." />
       <label>Object's opacity: </label>
   </service>
   @endcode
 *
 *@subsection In-Out In-Out
 * - \b material [sight::data::Material]: material object to update.
 *
 *@subsection Configuration Configuration
 * - \b label (optional, default="Material opacity : "): label of the slider.
 */

class MODULE_UI_QT_CLASS_API SMaterialOpacityEditor : public QObject,
                                                      public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SMaterialOpacityEditor, sight::ui::base::IEditor);

    /// Destroys the service.
    MODULE_UI_QT_API ~SMaterialOpacityEditor() noexcept override = default;

private:

    /// Configures the slider's label.
    void configuring() override;

    /// Sets the connections and the UI elements.
    void starting() override;

    /// Sets the opacity slider's value to the material's alpha value.
    void updating() override;

    /// Destroys the connections and cleans the container.
    void stopping() override;

    QPointer<QSlider> m_opacitySlider;
    QPointer<QLabel> m_opacityValue;

    /// Name that appears next to the slider.
    std::string m_label {"Material opacity : "};

    data::ptr<data::Material, data::Access::inout> m_material {this, "material", true};

private Q_SLOTS:

    /**
     * @brief Slot: called when the opacity slider changed.
     * @param _value The new opacity value.
     */
    void onOpacitySlider(int _value);
};

} // namespace sight::module::ui::qt