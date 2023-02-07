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

#include "Matrix4WriterTest.hpp"

#include <data/Matrix4.hpp>

#include <io/base/writer/Matrix4Writer.hpp>

#include <boost/algorithm/string.hpp>

#include <numeric>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::base::ut::Matrix4WriterTest);

namespace sight::io::base::ut
{

//------------------------------------------------------------------------------

void Matrix4WriterTest::basicTest()
{
    auto matrixWriter              = writer::Matrix4Writer::New();
    std::filesystem::path filepath = std::filesystem::temp_directory_path() / ("test" + matrixWriter->extension());
    auto matrixIn                  = data::Matrix4::New();
    std::iota(matrixIn->begin(), matrixIn->end(), 0);
    matrixWriter->setObject(matrixIn);
    std::filesystem::remove(filepath);
    matrixWriter->setFile(filepath);
    CPPUNIT_ASSERT_NO_THROW(matrixWriter->write());
    constexpr std::string_view expectedContent = R"(0 1 2 3
4 5 6 7
8 9 10 11
12 13 14 15)";
    std::string actualContent;
    {
        std::ifstream in(filepath);
        std::getline(in, actualContent, '\0');
        boost::trim(actualContent);
    }
    CPPUNIT_ASSERT_EQUAL(std::string(expectedContent), actualContent);
}

} // namespace sight::io::base::ut