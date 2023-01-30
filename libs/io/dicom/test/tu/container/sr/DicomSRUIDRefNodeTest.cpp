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

#include "DicomSRUIDRefNodeTest.hpp"

#include <io/dicom/container/sr/DicomSRUIDRefNode.hpp>
#include <io/dicom/helper/DicomDataReader.hxx>

#include <gdcmDataSet.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::sr::ut::DicomSRUIDRefNodeTest);

#define TYPE 0x0040, 0xa040
#define RELATIONSHIP 0x0040, 0xa010
#define UID 0x0040, 0xa124

namespace sight::io::dicom::container::sr::ut
{

//------------------------------------------------------------------------------

void DicomSRUIDRefNodeTest::basicTest()
{
    using namespace std::literals::string_literals;

    gdcm::DataSet dataset;
    DicomSRUIDRefNode({}, "friend", "uid").write(dataset);
    CPPUNIT_ASSERT_EQUAL("UIDREF"s, (io::dicom::helper::DicomDataReader::getTagValue<TYPE>(dataset)));
    CPPUNIT_ASSERT_EQUAL("friend"s, (io::dicom::helper::DicomDataReader::getTagValue<RELATIONSHIP>(dataset)));
    CPPUNIT_ASSERT_EQUAL("uid"s, (io::dicom::helper::DicomDataReader::getTagValue<UID>(dataset)));
}

} // namespace sight::io::dicom::container::sr::ut
