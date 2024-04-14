/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <io/http/request.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::http::ut
{

class request_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(request_test);
CPPUNIT_TEST(test_request);
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    // Creates a Request object
    void setUp() override;
    // Does nothing
    void tearDown() override;

    // Test functions
    // Tests the Request class API
    void test_request();

private:

    // Object Request
    sight::io::http::request::sptr m_request;
};

} // namespace sight::io::http::ut
