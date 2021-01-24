/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "WriterReaderTest.hpp"

#include <fwGdcmIO/reader/SeriesDB.hpp>
#include <fwGdcmIO/writer/Series.hpp>
#include <fwGdcmIO/writer/SeriesDB.hpp>

#include <core/tools/System.hpp>

#include <data/Boolean.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/generator/Object.hpp>
#include <fwTest/generator/SeriesDB.hpp>
#include <fwTest/helper/compare.hpp>
#include <fwTest/Slow.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGdcmIO::ut::WriterReaderTest );

namespace fwGdcmIO
{
namespace ut
{

//-----------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 1000. + .5) / 1000;
}

//------------------------------------------------------------------------------

void roundSpacing(data::Image::sptr image)
{
    data::Image::Spacing spacing = image->getSpacing2();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image->setSpacing2(spacing);
}

//------------------------------------------------------------------------------

void WriterReaderTest::setUp()
{
    // Set up context before running a test.
    if(::fwTest::Slow::ignoreSlowTests())
    {
        std::cout << std::endl << "Ignoring slow " << std::endl;
    }
    else
    {
        std::cout << std::endl << "Executing slow tests.." << std::endl;
    }
}

//------------------------------------------------------------------------------

void WriterReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void WriterReaderTest::writeReadImageSeriesTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwTest::generator::Image::initRand();
    data::ImageSeries::sptr imgSeries;
    imgSeries = ::fwTest::generator::SeriesDB::createImageSeries();

    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "dicomTest";

    std::filesystem::create_directories( PATH );

    ::fwGdcmIO::writer::Series::sptr writer = ::fwGdcmIO::writer::Series::New();
    writer->setObject(imgSeries);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    data::SeriesDB::sptr sdb = data::SeriesDB::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    std::filesystem::remove_all( PATH );

    // check series
    CPPUNIT_ASSERT_EQUAL(size_t(1), sdb->getContainer().size());

    data::Series::sptr series         = sdb->getContainer().front();
    data::ImageSeries::sptr imgseries = data::ImageSeries::dynamicCast(series);
    data::Image::sptr image           = imgseries->getImage();
    roundSpacing(image);

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(::fwTest::helper::compare(imgSeries, sdb->getContainer().front()));
}
//------------------------------------------------------------------------------

void WriterReaderTest::writeReadSeriesDBTest()
{
    if(::fwTest::Slow::ignoreSlowTests())
    {
        return;
    }
    ::fwTest::generator::Image::initRand();
    data::SeriesDB::sptr seriesDB;
    seriesDB = this->createSeriesDB();

    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "dicomTest";

    std::filesystem::create_directories( PATH );

    ::fwGdcmIO::writer::SeriesDB::sptr writer = ::fwGdcmIO::writer::SeriesDB::New();
    writer->setObject(seriesDB);
    writer->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(writer->write());

    // load ImageSeries
    data::SeriesDB::sptr sdb = data::SeriesDB::New();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    reader->setObject(sdb);
    reader->setFolder(PATH);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    std::filesystem::remove_all( PATH );

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT(::fwTest::helper::compare(seriesDB, sdb));
}

//------------------------------------------------------------------------------

data::SeriesDB::sptr WriterReaderTest::createSeriesDB()
{
    //create SeriesDB
    data::SeriesDB::sptr sdb            = data::SeriesDB::New();
    data::ImageSeries::sptr imgSeries   = ::fwTest::generator::SeriesDB::createImageSeries();
    data::ModelSeries::sptr modelSeries = ::fwTest::generator::SeriesDB::createModelSeries(1);

    sdb->getContainer().push_back(imgSeries);
    sdb->getContainer().push_back(modelSeries);

    data::Image::sptr image = imgSeries->getImage();

    // Add landmarks
    ::fwDataTools::fieldHelper::MedicalImageHelpers::checkLandmarks(image);
    data::PointList::sptr landmarks =
        image->getField< data::PointList >( ::fwDataTools::fieldHelper::Image::m_imageLandmarksId);
    const data::Image::Spacing spacing = image->getSpacing2();
    const data::Image::Origin origin   = image->getOrigin2();
    const data::Point::sptr point      = data::Point::New(2.6 + origin[0],
                                                          1.2 + origin[1],
                                                          4.5 + origin[2]);
    point->setField( ::fwDataTools::fieldHelper::Image::m_labelId, data::String::New("Label1") );
    landmarks->getPoints().push_back(point);
    data::Point::sptr point2 = data::Point::New(1.2 + origin[0],
                                                2.4 + origin[1],
                                                0.3 + origin[2]);
    point2->setField( ::fwDataTools::fieldHelper::Image::m_labelId, data::String::New("Label2") );
    landmarks->getPoints().push_back(point2);
    const data::Image::Size size   = image->getSize2();
    const data::Point::sptr point3 = data::Point::New(1.2 + origin[0],
                                                      2.4 + origin[1],
                                                      static_cast<double>(size[2]-1) * spacing[2] + origin[2]);
    point3->setField( ::fwDataTools::fieldHelper::Image::m_labelId, data::String::New("Label3") );
    landmarks->getPoints().push_back(point3);

    // Add distance
    data::PointList::sptr pl    = data::PointList::New();
    const data::Point::sptr pt1 = data::Point::New(0., 0., 0.);
    const data::Point::sptr pt2 = data::Point::New(static_cast<double>(size[0]-1) * spacing[0],
                                                   static_cast<double>(size[1]-1) * spacing[1],
                                                   static_cast<double>(size[2]-1) * spacing[2]);
    pl->getPoints().push_back( pt1 );
    pl->getPoints().push_back( pt2 );

    data::Vector::sptr vectDist;
    vectDist = image->setDefaultField< data::Vector >(
        ::fwDataTools::fieldHelper::Image::m_imageDistancesId, data::Vector::New());
    vectDist->getContainer().push_back(pl);

    image->setField("ShowLandmarks", data::Boolean::New(true));
    image->setField("ShowDistances", data::Boolean::New(true));

    // Update Reconstruction
    data::Reconstruction::sptr rec = modelSeries->getReconstructionDB().front();
    data::Mesh::sptr mesh          = rec->getMesh();
    mesh->clearCellColors();
    mesh->clearPointColors();
    mesh->clearCellNormals();

    // gdcm only manage ambient color in reconstruction
    data::Material::sptr material = data::Material::New();
    data::Color::sptr color       = ::fwTest::generator::Object::randomizeColor();
    material->setDiffuse(color);
    rec->setMaterial(material);
    rec->setImage(data::Image::sptr()); // not managed

    modelSeries->setField("ShowReconstructions", data::Boolean::New(true));

    return sdb;
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwGdcmIO
