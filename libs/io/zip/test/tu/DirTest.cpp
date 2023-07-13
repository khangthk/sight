/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "DirTest.hpp"

#include <core/os/TempPath.hpp>

#include <io/zip/ReadDirArchive.hpp>
#include <io/zip/WriteDirArchive.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::zip::ut::DirTest);

namespace sight::io::zip::ut
{

//------------------------------------------------------------------------------

void DirTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DirTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DirTest::writeReadFileTest()
{
    core::os::TempDir tmpDir;

    auto writer = std::make_shared<WriteDirArchive>(tmpDir);
    auto reader = std::make_shared<ReadDirArchive>(tmpDir);

    const std::filesystem::path testFile = "test.txt";

    CPPUNIT_ASSERT_NO_THROW(writer->createFile(testFile));

    CPPUNIT_ASSERT_NO_THROW(reader->getFile(testFile));
}

//------------------------------------------------------------------------------

void DirTest::writeDirTest()
{
    core::os::TempDir tmpDir;

    auto writer = std::make_shared<WriteDirArchive>(tmpDir);

    const std::filesystem::path testDir = "test";

    CPPUNIT_ASSERT_NO_THROW(writer->createDir(testDir));

    CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / testDir));
}

//------------------------------------------------------------------------------

void DirTest::putFileTest()
{
    core::os::TempDir tmpDir;

    auto writer = std::make_shared<WriteDirArchive>(tmpDir);

    const std::filesystem::path testDir = "test";
    //cspell: ignore makao
    const std::filesystem::path testFile = utestData::Data::dir() / "sight/image/jpg/makao01.jpg";
    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + testFile.string() + "' does not exist",
        std::filesystem::exists(testFile)
    );

    CPPUNIT_ASSERT_NO_THROW(writer->putFile(testFile, "image.jpg"));

    CPPUNIT_ASSERT(std::filesystem::exists(tmpDir / "image.jpg"));
}

} // namespace sight::io::zip::ut
