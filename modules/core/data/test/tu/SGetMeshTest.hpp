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

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::data::ut
{

/**
 * @brief Test Extract Mesh from Model series
 */
class SGetMeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SGetMeshTest);
CPPUNIT_TEST(extractsMeshByIndex);
CPPUNIT_TEST(extractsMeshWithIndexOutOfBound);
CPPUNIT_TEST(extractsMeshByType);
CPPUNIT_TEST(extractsMeshByTypeNotFound);
CPPUNIT_TEST(extractsMeshByNameNotFound);
CPPUNIT_TEST(extractsWithInvalidModelsSeries);
CPPUNIT_TEST(extractsWithMeshTypeAndIndex);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void extractsMeshByIndex();
    static void extractsMeshByType();
    static void extractsMeshByTypeNotFound();
    static void extractsMeshByNameNotFound();
    static void extractsWithInvalidModelsSeries();
    static void extractsMeshWithIndexOutOfBound();
    static void extractsWithMeshTypeAndIndex();
};

} // namespace sight::module::data::ut
