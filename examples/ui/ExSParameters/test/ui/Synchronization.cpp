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

// cspell:ignore exsparameters

#include "Synchronization.hpp"

#include <core/runtime/path.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/CheckBox.hpp>
#include <ui/testCore/helper/ColorParameter.hpp>
#include <ui/testCore/helper/ComboBox.hpp>
#include <ui/testCore/helper/Label.hpp>
#include <ui/testCore/helper/Slider.hpp>
#include <ui/testCore/helper/SpinBox.hpp>
#include <ui/testCore/Tester.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::examples::ui::exsparameters::test::ui::Synchronization);

namespace sight::examples::ui::exsparameters::test::ui
{

//------------------------------------------------------------------------------

std::filesystem::path Synchronization::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::getWorkingPath();
    return cwd / "share/sight/ExSParameters/profile.xml";
}

//------------------------------------------------------------------------------

void Synchronization::test()
{
    namespace helper = sight::ui::testCore::helper;
    using Select     = helper::Select;

    start(
        "Synchronization",
        [](sight::ui::testCore::Tester& tester)
        {
            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Enable' checkboxes");
                helper::CheckBox::toggle(tester, Select::fromParent("parameters1Srv", "enable"));
                helper::CheckBox::shouldNotBeChecked(tester, Select::current());
                helper::CheckBox::shouldNotBeChecked(tester, Select::fromParent("parameters2Srv", "enable"));
            }

            {
                auto bt =
                    tester.addInBacktrace("Check that 'Boolean' checkbox doesn't do anything because it is disabled");
                helper::CheckBox::toggle(tester, Select::fromParent("parameters1Srv", "bool_1"));
                helper::CheckBox::shouldNotBeChecked(tester, Select::current());
            }

            {
                auto bt = tester.addInBacktrace("Re-enable the parameters by re-checking the 'Enable' checkboxes");
                helper::CheckBox::toggle(tester, Select::fromParent("parameters1Srv", "enable"));
                helper::CheckBox::shouldBeChecked(tester, Select::current());
                helper::CheckBox::shouldBeChecked(tester, Select::fromParent("parameters2Srv", "enable"));
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Boolean' checkboxes");
                helper::CheckBox::toggle(tester, Select::fromParent("parameters1Srv", "bool_1"));
                helper::CheckBox::shouldBeChecked(tester, Select::current());
                helper::CheckBox::shouldBeChecked(tester, Select::fromParent("parameters2Srv", "bool_1"));
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Color' color selectors");
                helper::ColorParameter::select(tester, Select::fromParent("parameters1Srv", "color_1"), {0, 0, 0});
                helper::ColorParameter::colorEquals(tester, Select::fromParent("parameters1Srv", "color_1"), {0, 0, 0});
                helper::ColorParameter::colorEquals(tester, Select::fromParent("parameters2Srv", "color_1"), {0, 0, 0});
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Double 1' spinboxes");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "double_1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), "0.40");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double_1"), "0.40");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Double 2' sliders");
                helper::Slider::move(
                    tester,
                    Select::fromParent("parameters1Srv", "double_2"),
                    helper::Slider::Position::RIGHT,
                    5
                );
                helper::Label::exactlyMatch(
                    tester,
                    Select::fromParent(
                        "parameters1Srv",
                        "double_2/valueLabel"
                    ),
                    "1.00"
                );
                helper::Label::exactlyMatch(
                    tester,
                    Select::fromParent(
                        "parameters2Srv",
                        "double_2/valueLabel"
                    ),
                    "1.00"
                );
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between '2 Double' spinboxes");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "double2_1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), "55.000");
                helper::SpinBox::decrement(tester, Select::fromParent("parameters1Srv", "double2_1/1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), "45.000");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double2_1"), "55.000");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double2_1/1"), "45.000");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between '3 Double' spinboxes");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "double3_1"), 4);
                helper::SpinBox::valueEquals(tester, Select::current(), "29.000");
                helper::SpinBox::decrement(tester, Select::fromParent("parameters1Srv", "double3_1/1"), 4);
                helper::SpinBox::valueEquals(tester, Select::current(), "21.000");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "double3_1/2"), 4);
                helper::SpinBox::valueEquals(tester, Select::current(), "29.000");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double3_1"), "29.000");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double3_1/2"), "29.000");
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "double3_1/1"), "21.000");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Integer 1' spinboxes");
                helper::SpinBox::decrement(tester, Select::fromParent("parameters1Srv", "int_1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 15);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int_1"), 15);
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Integer 2' sliders");
                helper::Slider::move(
                    tester,
                    Select::fromParent("parameters1Srv", "int_2"),
                    helper::Slider::Position::LEFT,
                    2
                );
                helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "int_2/valueLabel"), "0");
                helper::Label::exactlyMatch(tester, Select::fromParent("parameters2Srv", "int_2/valueLabel"), "0");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between '2 Integer' spinboxes");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "int2_1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 13);
                helper::SpinBox::decrement(tester, Select::fromParent("parameters1Srv", "int2_1/1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 3);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int2_1"), 13);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int2_1/1"), 3);
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between '3 Integer' spinboxes");
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "int3_1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 17);
                helper::SpinBox::decrement(tester, Select::fromParent("parameters1Srv", "int3_1/1"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 7);
                helper::SpinBox::increment(tester, Select::fromParent("parameters1Srv", "int3_1/2"), 5);
                helper::SpinBox::valueEquals(tester, Select::current(), 17);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int3_1"), 17);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int3_1/1"), 7);
                helper::SpinBox::valueEquals(tester, Select::fromParent("parameters2Srv", "int3_1/2"), 17);
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization between 'Enum' comboboxes");
                helper::ComboBox::select(tester, Select::fromParent("parameters1Srv", "enum_1"), "V1");
                helper::ComboBox::valueEquals(tester, Select::current(), "V1");
                helper::ComboBox::valueEquals(tester, Select::fromParent("parameters2Srv", "enum_1"), "V1");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization from 'Nonlinear' non-linear slider to combobox");
                helper::Slider::move(
                    tester,
                    Select::fromParent(
                        "parameters1Srv",
                        "nonlin"
                    ),
                    helper::Slider::Position::RIGHT
                );
                helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/valueLabel"), "150");
                helper::ComboBox::valueEquals(tester, Select::fromParent("parameters2Srv", "nonlin"), "150");
            }

            {
                auto bt = tester.addInBacktrace("Check synchronization from 'Nonlinear' combobox to non-linear slider");
                helper::ComboBox::select(tester, Select::fromParent("parameters2Srv", "nonlin"), "120");
                helper::ComboBox::valueEquals(tester, Select::current(), "120");
                helper::Label::exactlyMatch(tester, Select::fromParent("parameters1Srv", "nonlin/valueLabel"), "120");
            }
        },
        true
    );
}

} // namespace sight::examples::ui::exsparameters::test::ui
