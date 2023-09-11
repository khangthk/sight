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

#include "Select.hpp"

#include <ui/testCore/Tester.hpp>

namespace sight::ui::testCore::helper
{

class Label
{
public:

    /**
     * Check whether the tooltip of a label exactly matches the expected result.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected content of the tooltip
     */
    GUITEST_API static void toolTipMatches(Tester& tester, const Select& label, const std::string& expected);

    /**
     * Check whether the label contains the expected substring.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The substring expected to be contained in the label
     */
    GUITEST_API static void contain(Tester& tester, const Select& label, const std::string& expected);

    /**
     * Check whether the label exactly matches the expected result.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected content of the label
     */
    GUITEST_API static void exactlyMatch(Tester& tester, const Select& label, const std::string& expected);

    /**
     * Check whether the label is equal to the number with a tolerance.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected value of the label
     * @param tolerance The tolerance for the expected value
     * @param re A regexp describing how to find the number to be checked in the label, it should be the first capture
     *           group.
     */
    GUITEST_API static void equal(
        Tester& tester,
        const Select& label,
        double expected,
        double tolerance,
        QRegExp re = QRegExp("(.*)")
    );

    /**
     * Get the value within a label.
     * @param tester The current tester
     * @param label The objectName of the label
     */
    GUITEST_API static std::string get(Tester& tester, const Select& labelName);
};

} // namespace sight::ui::testCore::helper