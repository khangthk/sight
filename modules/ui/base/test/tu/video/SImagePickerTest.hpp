/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <service/IService.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::base::video::ut
{

class SImagePickerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SImagePickerTest);
CPPUNIT_TEST(basicTest);
CPPUNIT_TEST(clickWithoutControlTest);
CPPUNIT_TEST(topLeftRefTest);
CPPUNIT_TEST(singlePointModeTest);
CPPUNIT_TEST(noCtrlModifierTest);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    void basicTest();
    void clickWithoutControlTest();
    void topLeftRefTest();
    void singlePointModeTest();
    void noCtrlModifierTest();

private:

    service::IService::sptr m_imagePicker;
};

} // namespace sight::module::ui::base::video::ut
