/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <cppunit/extensions/HelperMacros.h>

namespace fwStructuralPatch
{
namespace ut
{
namespace fwMedData
{
namespace NavigationSeries
{

/// Test structural patch to convert a fwMedData::NavigationSeries from version '1' to version '2'.
class V1ToV2Test : public CPPUNIT_NS::TestFixture
{

CPPUNIT_TEST_SUITE( V1ToV2Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:

    /// Sets up context before running a test.
    void setUp();

    /// Cleans up after the test run.
    void tearDown();

    void applyPatchTest();
};

} //namespace NavigationSeries
} //namespace fwMedData
} //namespace ut
} //namespace fwStructuralPatch