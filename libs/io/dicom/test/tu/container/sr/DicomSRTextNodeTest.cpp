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

#include "DicomSRTextNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRTextNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmDataSet.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRTextNodeTest);

#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010
#define TEXT 0x0040, 0xa160

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void DicomSRTextNodeTest::basicTest()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    DicomSRTextNode({}, "friend", "hello world").write(dataset);
    CPPUNIT_ASSERT_EQUAL("TEXT"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL("hello world"s, (io::dicom::helper::DicomDataReader::getTagValue<TEXT>(dataset)));
}

} // namespace sight::io::dicom::container::sr::ut
