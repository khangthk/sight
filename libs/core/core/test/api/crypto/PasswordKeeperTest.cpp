/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "PasswordKeeperTest.hpp"

#include <core/crypto/PasswordKeeper.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::crypto::ut::PasswordKeeperTest);

namespace sight::core::crypto
{

namespace ut
{

//------------------------------------------------------------------------------

void PasswordKeeperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::tearDown()
{
    // Clean up after the test run.
    // Reset global static password
    core::crypto::PasswordKeeper::set_global_password("");
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::globalTest()
{
    {
        // Set a dummy "abc" global password
        const core::crypto::PasswordKeeper globalKeeper;
        globalKeeper.set_global_password("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, globalKeeper.get_global_password_hash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), globalKeeper.get_global_password());
        CPPUNIT_ASSERT(globalKeeper.check_global_password("abc"));
    }

    {
        // Test that we can get the "abc" password back, even with an other PasswordKeeper
        const core::crypto::PasswordKeeper globalKeeper;
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), globalKeeper.get_global_password());
        CPPUNIT_ASSERT(globalKeeper.check_global_password("abc"));
    }
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::localTest()
{
    {
        // Set a dummy "abc" global password
        core::crypto::PasswordKeeper localKeeper;
        localKeeper.set_password("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, localKeeper.get_password_hash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), localKeeper.get_password());
        CPPUNIT_ASSERT(localKeeper.check_password("abc"));
    }

    {
        // Test that we can NOT get the "abc" password back with an other PasswordKeeper
        const core::crypto::PasswordKeeper localKeeper;
        CPPUNIT_ASSERT(!localKeeper.check_password("abc"));
    }
}

} // namespace ut

} // namespace sight::core::crypto
