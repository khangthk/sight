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

// cspell:ignore Doesnt

#pragma once

#include <service/IService.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::ui::base::com::ut
{

class DummyService;

class SStarterTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SStarterTest);
CPPUNIT_TEST(startTest);
CPPUNIT_TEST(startOnlyTest);
CPPUNIT_TEST(startIfExistsTest);
CPPUNIT_TEST(startIfExistsButDoesntExistTest);
CPPUNIT_TEST(stopTest);
CPPUNIT_TEST(stopIfExistsTest);
CPPUNIT_TEST(stopIfExistsButDoesntExistTest);
CPPUNIT_TEST(startOrStopTest);
CPPUNIT_TEST(startOnlyOrStopTest);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;

    enum class ShouldBe
    {
        STARTED,
        STOPPED,
        STARTED_THEN_STOPPED
    };

    void test(const std::string& actionName, bool exists, ShouldBe shouldBe = ShouldBe::STARTED, int nbUpdate = 0);

    void startTest();
    void startOnlyTest();
    void startIfExistsTest();
    void startIfExistsButDoesntExistTest();
    void stopTest();
    void stopIfExistsTest();
    void stopIfExistsButDoesntExistTest();
    void startOrStopTest();
    void startOnlyOrStopTest();

private:

    service::IService::sptr m_starter;
    std::shared_ptr<DummyService> m_dummyService;
};

} // namespace sight::module::ui::base::com::ut