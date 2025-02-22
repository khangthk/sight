/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#pragma once

#include <core/com/signals.hpp>
#include <core/com/slots.hpp>
#include <core/tools/failed.hpp>

#include <data/string.hpp>

#include <ui/__/action.hpp>
#include <ui/__/parameter.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>

#include <vector>

namespace sight::data
{

class String;

} // namespace sight::data

namespace sight::module::ui::qt
{

/**
 * @brief This action shows a dialog to configure preferences of an application.
 *
 * @note This service works with preferences and does nothing if the preferences service is not found.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::ui::qt::preferences_configuration">
        <preference>
            <type>path</type>
            <name>Video path</name>
            <key>VIDEO_REF_DIR</key>
            <default_value>...</default_value>
        </preference>
        <preference>
            <type>file</type>
            <name>Video file</name>
            <key>VIDEO_REF_FILE</key>
            <default_value>...</default_value>
        </preference>
        <preference>
            <type>text</type>
            <name>DICOM/MPPS port</name>
            <key>DICOM_PORT_KEY</key>
            <default_value>11112</default_value>
        </preference>
        <preference>
            <type>text</type>
            <name>AE Title</name>
            <key>DICOM_AETITLE_KEY</key>
            <default_value>dicom</default_value>
        </preference>
        <preference>
            <type>checkbox</type>
            <name>Keep PixelData (C-STORE)</name>
            <key>KEEP_PIXELDATA_KEY</key>
            <default_value>true</default_value>
        </preference>
        <preference>
            <type>combobox</type>
            <name>Device name</name>
            <key>DEVICE_NAME</key>
            <values>trakStar,Aurora</values>
            <default_value>trakStar</default_value>
        </preference>
        <preference>
            <min>50</min>
            <max>500</max>
            <type>int</type>
            <name>Device port</name>
            <key>DEVICE_PORT</key>
            <default_value>104</default_value>
        </preference>
        <preference>
            <type>list</type>
            <name>Values for Nonlinear</name>
            <key>nonlin</key>
            <separator>,</separator>
            <default_value>25,50,90,120,150</default_value>
        </preference>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b type (mandatory, path/text/checkbox/int/double/combobox): the type of the parameter field.
 * - \b name (mandatory, string): the name of the parameter.
 * - \b key (mandatory, string): the key of the parameter.
 * - \b default_value (mandatory): the default value of the parameter.
 * - \b values (optional): list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for
 *                         'combobox' type).
 * - \b min (optional, int/double, default=0/-1000000.0): minimum value allowed in the field.
 * - \b max (optional, int/double, default=999999/1000000.0): maximum value allowed in the field.
 * - \b separator (optional, list, default=','): the separator for the list type
 */
class preferences_configuration final : public QObject,
                                        public sight::ui::action
{
Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(preferences_configuration, sight::ui::action);

    /// Initializes the signal.
    preferences_configuration() noexcept;

    /// Destroys the service.
    ~preferences_configuration() noexcept override;

protected:

    /// Configures the service.
    void configuring() override;

    /// Gets the preference map.
    void starting() override;

    /// Shows a dialog to configure preferences declared in xml.
    void updating() override;

    /// Does nothing.
    void stopping() override;

    /// Does nothing.
    void request_values();

private Q_SLOTS:

    void on_reset_to_default_value(QObject* _widget);

private:

    /// Type of signal when parameters are updated.
    using parameters_modified_signal_t = core::com::signal<void ()>;
    static const core::com::signals::key_t PARAMETERS_MODIFIED_SIG;

    /// Generic changed signal type
    using changed_signal_t = core::com::signal<void (sight::ui::parameter_t, std::string)>;
    static const core::com::signals::key_t PREFERENCE_CHANGED_SIG;

    /// Internal wrapper holding slots keys.
    struct slots
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t REQUEST_VALUES = "request_values";
    };

    enum class preference_t : std::int8_t
    {
        text,
        checkbox,
        INT,
        path,
        file,
        combobox,
        DOUBLE,
        list
    };

    struct preference_elt
    {
        preference_t m_type {preference_t::text};
        QPointer<QLineEdit> m_line_edit;
        QPointer<QCheckBox> m_check_box;
        QPointer<QComboBox> m_combo_box;
        std::string m_preference_value;
        std::string m_preference_key;
        std::string m_name;
        std::string m_default_value;
        std::pair<int, int> m_i_min_max {0, 999999};
        std::pair<double, double> m_d_min_max {-1000000.0, 1000000.0};
        std::string m_separator;
        std::vector<std::string> m_values;
    };

    /// @brief Converts string value from PreferenceElt.m_preferenceValue to real type regarding PreferenceElt.m_type.
    /// @param _elt The preference element
    /// @return std::variant as defined by parameter_t.
    static sight::ui::parameter_t convert_value(const preference_elt& _elt);

    static void on_select_dir(QPointer<QLineEdit> _line_edit);

    static void on_select_file(QPointer<QLineEdit> _line_edit);

    void update_from_preferences() noexcept;

    parameters_modified_signal_t::sptr m_sig_parameters_modified;
    changed_signal_t::sptr m_sig_preference_changed;

    std::vector<preference_elt> m_preferences;
};

} // namespace sight::module::ui::qt
