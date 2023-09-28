/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "SessionTest.hpp"

#include <core/crypto/aes256.hpp>
#include <core/crypto/base64.hpp>
#include <core/os/temp_path.hpp>
#include <core/tools/uuid.hpp>

#include <data/Activity.hpp>
#include <data/ActivitySet.hpp>
#include <data/Array.hpp>
#include <data/Boolean.hpp>
#include <data/CalibrationInfo.hpp>
#include <data/Camera.hpp>
#include <data/CameraSet.hpp>
#include <data/Color.hpp>
#include <data/Composite.hpp>
#include <data/DicomSeries.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/Landmarks.hpp>
#include <data/Line.hpp>
#include <data/Material.hpp>
#include <data/Matrix4.hpp>
#include <data/ModelSeries.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/Plane.hpp>
#include <data/PlaneList.hpp>
#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/Reconstruction.hpp>
#include <data/Resection.hpp>
#include <data/ResectionDB.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>
#include <data/Set.hpp>
#include <data/String.hpp>
#include <data/StructureTraits.hpp>
#include <data/StructureTraitsDictionary.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <geometry/data/Mesh.hpp>

#include <io/dicom/reader/SeriesSet.hpp>
#include <io/session/detail/core/SessionDeserializer.hpp>
#include <io/session/detail/core/SessionSerializer.hpp>
#include <io/session/Helper.hpp>
#include <io/session/SessionReader.hpp>
#include <io/session/SessionWriter.hpp>
#include <io/zip/exception/Read.hpp>
#include <io/zip/exception/Write.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>
#include <utestData/generator/Mesh.hpp>

#include <random>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::session::ut::SessionTest);

namespace sight::io::session::ut
{

// For UUID::generate();
using core::tools::UUID;

//------------------------------------------------------------------------------

template<typename T>
inline T random()
{
    using uniform_distribution = std::conditional_t<
        std::is_floating_point_v<T>,
        std::uniform_real_distribution<T>,
        std::uniform_int_distribution<T>
    >;

    static uniform_distribution distributor(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
    static std::mt19937 generator {std::random_device {}()};

    return distributor(generator);
}

//------------------------------------------------------------------------------

inline static std::string generateTM(std::size_t variant)
{
    std::stringstream hh;
    hh << std::setfill('0') << std::setw(2) << (variant % 24);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 60);

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << ((variant + 2) % 60);

    std::stringstream ffffff;
    ffffff << std::setfill('0') << std::setw(6) << ((variant + 3) % 1000000);

    return hh.str() + mm.str() + ss.str() + "." + ffffff.str();
}

//------------------------------------------------------------------------------

inline static std::string generateDA(std::size_t variant)
{
    std::stringstream yyyy;
    yyyy << std::setfill('0') << std::setw(4) << (variant % 10000);

    std::stringstream mm;
    mm << std::setfill('0') << std::setw(2) << ((variant + 1) % 12);

    std::stringstream dd;
    dd << std::setfill('0') << std::setw(2) << ((variant + 2) % 32);

    return yyyy.str() + mm.str() + dd.str();
}

//------------------------------------------------------------------------------

inline static std::string generateDT(std::size_t variant)
{
    return generateDA(variant) + generateTM(variant);
}

//------------------------------------------------------------------------------

void SessionTest::setUp()
{
}

//------------------------------------------------------------------------------

void SessionTest::tearDown()
{
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr generate(const std::size_t /*unused*/)
{
    return std::make_shared<T>(static_cast<typename T::ValueType>(random<typename T::ValueType>()));
}

//------------------------------------------------------------------------------

template<typename T>
inline const typename T::csptr& getExpected(const std::size_t variant)
{
    static std::map<std::size_t, typename T::csptr> MAP;
    const auto& it = MAP.find(variant);

    if(it == MAP.cend())
    {
        const auto& object = generate<T>(variant);
        object->setDescription(UUID::generate());

        return MAP.insert_or_assign(variant, object).first->second;
    }

    return it->second;
}

//------------------------------------------------------------------------------

template<typename T>
inline typename T::sptr create(const std::size_t variant)
{
    const auto& object = std::make_shared<T>();
    object->deep_copy(getExpected<T>(variant));
    return object;
}

//------------------------------------------------------------------------------

template<typename T>
inline void compare(const typename T::csptr& actual, const std::size_t variant)
{
    CPPUNIT_ASSERT(actual);
    CPPUNIT_ASSERT_EQUAL(getExpected<T>(variant)->getValue(), actual->getValue());
}

//------------------------------------------------------------------------------

template<typename T>
inline void test(const bool encrypt, const bool raw, const bool empty_obj = false)
{
    static constexpr auto password = "password";

    const auto& test_id = T::leaf_classname() + "_" + std::to_string(encrypt) + "_" + std::to_string(raw);

    // Create a temporary directory
    core::os::temp_dir tmpDir;
    const auto testPath = tmpDir / (test_id + (raw ? ".json" : ".zip"));

    static constexpr auto fieldName = "field";

    // Test serialization
    {
        // Create the data object
        auto object = empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        object->setField(fieldName, create<T>(1));

        // Create the session writer
        auto sessionWriter = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(object);
        sessionWriter->set_file(testPath);

        if(raw)
        {
            sessionWriter->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionWriter->setPassword(password);
        }

        // Write the session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->set_file(testPath);

        if(raw)
        {
            sessionReader->setArchiveFormat(io::zip::Archive::ArchiveFormat::FILESYSTEM);
        }
        else if(encrypt)
        {
            sessionReader->setPassword(password);
        }

        // Read the session
        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto actual_object = std::dynamic_pointer_cast<T>(sessionReader->getObject());

        // Create the data object
        auto expected_object = empty_obj ? std::make_shared<T>() : create<T>(0);

        // Add a field
        expected_object->setField(fieldName, create<T>(1));

        CPPUNIT_ASSERT(*expected_object == *actual_object);
    }
}

//------------------------------------------------------------------------------

template<typename T>
inline void testCombine()
{
    test<T>(false, false, false);
    test<T>(false, false, true);
    test<T>(false, true, false);
    test<T>(false, true, true);
    test<T>(true, false, false);
    test<T>(true, false, true);
}

//------------------------------------------------------------------------------

template<>
inline data::Boolean::sptr generate<data::Boolean>(const std::size_t variant)
{
    return std::make_shared<data::Boolean>(variant % 2 == 0);
}

//------------------------------------------------------------------------------

void SessionTest::booleanTest()
{
    testCombine<data::Boolean>();
}

//------------------------------------------------------------------------------

void SessionTest::integerTest()
{
    testCombine<data::Integer>();
}

//------------------------------------------------------------------------------

void SessionTest::floatTest()
{
    testCombine<data::Float>();
}

//------------------------------------------------------------------------------

template<>
inline data::String::sptr generate<data::String>(const std::size_t /*unused*/)
{
    return std::make_shared<data::String>(UUID::generate());
}

//------------------------------------------------------------------------------

void SessionTest::stringTest()
{
    testCombine<data::String>();
}

//------------------------------------------------------------------------------

template<>
inline data::Composite::sptr generate<data::Composite>(const std::size_t variant)
{
    auto object = std::make_shared<data::Composite>();
    (*object)[data::Boolean::classname()] = create<data::Boolean>(variant);
    (*object)[data::Integer::classname()] = create<data::Integer>(variant);
    (*object)[data::Float::classname()]   = create<data::Float>(variant);
    (*object)[data::String::classname()]  = create<data::String>(variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::compositeTest()
{
    testCombine<data::Composite>();
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr create<data::Mesh>(const std::size_t variant)
{
    const auto& object = std::make_shared<data::Mesh>();
    object->deep_copy(getExpected<data::Mesh>(variant));
    object->shrinkToFit();
    return object;
}

//------------------------------------------------------------------------------

template<>
inline data::Mesh::sptr generate<data::Mesh>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::Mesh>();

    utestData::generator::Mesh::generateTriangleQuadMesh(object);
    geometry::data::Mesh::shakePoint(object);
    geometry::data::Mesh::colorizeMeshPoints(object);
    geometry::data::Mesh::colorizeMeshCells(object);
    geometry::data::Mesh::generatePointNormals(object);
    geometry::data::Mesh::generateCellNormals(object);
    object->shrinkToFit();

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::meshTest()
{
    testCombine<data::Mesh>();
}

//------------------------------------------------------------------------------

template<>
inline data::Series::sptr generate<data::Series>(const std::size_t variant)
{
    auto object = std::make_shared<data::Series>();

    // Fill trivial attributes
    object->setSOPKeyword(sight::data::dicom::sop::Keyword::CTImageStorage);
    object->setModality(UUID::generate());
    object->setSeriesDescription(UUID::generate());
    object->setSeriesInstanceUID(UUID::generate());
    object->setSeriesNumber(std::int32_t(variant));
    object->setLaterality(UUID::generate());
    object->setSeriesDate(generateDA(variant));
    object->setSeriesTime(generateTM(variant));
    object->setPerformingPhysicianName(UUID::generate() + "\\" + UUID::generate());
    object->setProtocolName(UUID::generate());
    object->setBodyPartExamined(UUID::generate());
    object->setPatientPosition(UUID::generate());
    object->setAnatomicalOrientationType(UUID::generate());
    object->setPerformedProcedureStepID(UUID::generate());
    object->setPerformedProcedureStepStartDate(generateDA(variant));
    object->setPerformedProcedureStepStartTime(generateTM(variant));
    object->setPerformedProcedureStepEndDate(generateDA(variant));
    object->setPerformedProcedureStepEndTime(generateTM(variant));
    object->setPerformedProcedureStepDescription(UUID::generate());
    object->setCommentsOnThePerformedProcedureStep(UUID::generate());

    // Equipment Module
    object->setInstitutionName(UUID::generate());

    // Patient Module
    object->setPatientName(UUID::generate());
    object->setPatientID(UUID::generate());
    object->setPatientBirthDate(generateDA(variant));
    object->setPatientSex(UUID::generate());

    // Study Module
    object->setStudyDescription(UUID::generate());
    object->setStudyInstanceUID(UUID::generate());
    object->setStudyID(UUID::generate());
    object->setStudyDate(generateDA(variant));
    object->setStudyTime(generateTM(variant));
    object->setReferringPhysicianName(UUID::generate());

    // Patient Study Module
    object->setPatientAge(UUID::generate());
    object->setPatientSize(double(variant));
    object->setPatientWeight(double(variant));

    // Generate specific instance data
    for(std::size_t i = 0 ; i < variant + 3 ; ++i)
    {
        object->setImagePositionPatient(
            {
                double(variant + i),
                double(variant + i + 1),
                double(variant + i + 2)
            },
            i
        );

        object->setImageOrientationPatient(
            {
                double(variant + i), double(variant + i + 1), double(variant + i + 2),
                double(variant + i + 3), double(variant + i + 4), double(variant + i + 5)
            },
            i
        );

        object->setFrameAcquisitionDateTime(generateDT(variant + i), i);
        object->setFrameComments(UUID::generate(), i);
        object->setFrameLabel(UUID::generate(), i);
    }

    // Test private tag...
    object->setPrivateValue(UUID::generate(), 0x10);
    object->setMultiFramePrivateValue(UUID::generate(), 0x15, 0);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::seriesTest()
{
    testCombine<data::Series>();
}

//------------------------------------------------------------------------------

template<>
inline data::Activity::sptr generate<data::Activity>(const std::size_t variant)
{
    auto object = std::make_shared<data::Activity>();

    (*object)[data::Boolean::classname()] = create<data::Boolean>(variant);
    (*object)[data::Integer::classname()] = create<data::Integer>(variant);
    (*object)[data::Float::classname()]   = create<data::Float>(variant);
    (*object)[data::String::classname()]  = create<data::String>(variant);

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::activityTest()
{
    testCombine<data::Activity>();
}

//------------------------------------------------------------------------------

template<>
inline data::Array::sptr generate<data::Array>(const std::size_t variant)
{
    auto object = std::make_shared<data::Array>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            object->resize(
                {variant + 2, variant + 2},
                std::is_same_v<T, double>
                ? core::type::DOUBLE
                : std::is_same_v<T, float>
                ? core::type::FLOAT
                : std::is_same_v<T, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<T, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<T, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<T, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<T, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<T, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<T, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<T, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,
                true
            );

            T counter = static_cast<T>(0);
            for(auto it = object->begin<T>(),
                end = object->end<T>() ;
                it != end ;
                ++it)
            {
                *it = static_cast<T>(variant) + counter++;
            }
        };

    switch(variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::arrayTest()
{
    testCombine<data::Array>();
}

//------------------------------------------------------------------------------

template<>
inline data::Image::sptr generate<data::Image>(const std::size_t variant)
{
    auto object = std::make_shared<data::Image>();

    const auto lock = object->dump_lock();

    auto fill =
        [&](auto type)
        {
            using T = decltype(type);

            // Warning: generateImage use reflection that cannot deal with double value (truncated to float precision),
            // thus the 0.1 + static_cast<double>(variant)
            utestData::generator::Image::generateImage(
                object,
                {variant + 5, variant + 5, variant + 5},
            {
                0.1 + static_cast<double>(variant),
                0.2 + static_cast<double>(variant),
                0.3 + static_cast<double>(variant)
            },
            {
                0.4 + static_cast<double>(variant),
                0.5 + static_cast<double>(variant),
                0.6 + static_cast<double>(variant)
            },
                std::is_same_v<T, double>
                ? core::type::DOUBLE
                : std::is_same_v<T, float>
                ? core::type::FLOAT
                : std::is_same_v<T, std::uint8_t>
                ? core::type::UINT8
                : std::is_same_v<T, std::uint16_t>
                ? core::type::UINT16
                : std::is_same_v<T, std::uint32_t>
                ? core::type::UINT32
                : std::is_same_v<T, std::uint64_t>
                ? core::type::UINT64
                : std::is_same_v<T, std::int8_t>
                ? core::type::INT8
                : std::is_same_v<T, std::int16_t>
                ? core::type::INT16
                : std::is_same_v<T, std::int32_t>
                ? core::type::INT32
                : std::is_same_v<T, std::int64_t>
                ? core::type::INT64
                : core::type::NONE,

                std::is_same_v<T, double>
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, float>
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, std::uint8_t>
                ? data::Image::PixelFormat::BGR
                : std::is_same_v<T, std::uint16_t>
                ? data::Image::PixelFormat::BGRA
                : std::is_same_v<T, std::uint32_t>
                ? data::Image::PixelFormat::RGB
                : std::is_same_v<T, std::uint64_t>
                ? data::Image::PixelFormat::RGBA
                : std::is_same_v<T, std::int8_t>
                ? data::Image::PixelFormat::GRAY_SCALE
                : std::is_same_v<T, std::int16_t>
                ? data::Image::PixelFormat::BGR
                : std::is_same_v<T, std::int32_t>
                ? data::Image::PixelFormat::BGRA
                : std::is_same_v<T, std::int64_t>
                ? data::Image::PixelFormat::RGB
                : data::Image::PixelFormat::UNDEFINED,
                std::uint32_t(variant)
            );
        };

    switch(variant % 5)
    {
        case 0:
            fill(static_cast<std::uint8_t>(0));
            break;

        case 1:
            fill(static_cast<std::uint16_t>(0));
            break;

        case 2:
            fill(static_cast<std::uint32_t>(0));
            break;

        case 3:
            fill(0.0);
            break;

        case 4:
            fill(0.0F);
            break;

        default:
            CPPUNIT_FAIL("Unknown variant.");
            break;
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::imageTest()
{
    testCombine<data::Image>();
}

//------------------------------------------------------------------------------

template<>
inline data::Vector::sptr generate<data::Vector>(const std::size_t variant)
{
    auto object = std::make_shared<data::Vector>();

    object->push_back(create<data::Boolean>(variant));
    object->push_back(create<data::Integer>(variant));
    object->push_back(create<data::Float>(variant));
    object->push_back(create<data::String>(variant));
    object->push_back(create<data::Activity>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::vectorTest()
{
    testCombine<data::Vector>();
}

//------------------------------------------------------------------------------

template<>
inline data::Point::sptr generate<data::Point>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::Point>();

    object->setCoord({random<double>(), random<double>(), random<double>()});

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::pointTest()
{
    testCombine<data::Point>();
}

//------------------------------------------------------------------------------

template<>
inline data::PointList::sptr generate<data::PointList>(const std::size_t variant)
{
    auto object = std::make_shared<data::PointList>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = variant + 3 ; i < end ; ++i)
    {
        points.push_back(create<data::Point>(i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::pointListTest()
{
    testCombine<data::PointList>();
}

//------------------------------------------------------------------------------

template<>
inline data::CalibrationInfo::sptr generate<data::CalibrationInfo>(const std::size_t variant)
{
    auto object = std::make_shared<data::CalibrationInfo>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        // Create the Image
        auto image = create<data::Image>(variant + i);

        // Create the PointList
        auto pointList = create<data::PointList>(variant + i);

        object->addRecord(image, pointList);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::calibrationInfoTest()
{
    testCombine<data::CalibrationInfo>();
}

//------------------------------------------------------------------------------

template<>
inline data::Camera::sptr generate<data::Camera>(const std::size_t variant)
{
    auto object = std::make_shared<data::Camera>();

    object->setWidth(random<std::size_t>());
    object->setHeight(random<std::size_t>());
    object->setFx(random<double>());
    object->setFy(random<double>());
    object->setCx(random<double>());
    object->setCy(random<double>());
    object->setDistortionCoefficient(
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>(),
        random<double>()
    );
    object->setSkew(random<double>());
    object->setIsCalibrated(variant % 2 == 0);
    object->setCameraID(UUID::generate());
    object->setMaximumFrameRate(random<float>());
    constexpr std::array pixelFormats {
        data::Camera::PixelFormat::ADOBEDNG,
        data::Camera::PixelFormat::ARGB32,
        data::Camera::PixelFormat::ARGB32_PREMULTIPLIED,
        data::Camera::PixelFormat::RGB32,
        data::Camera::PixelFormat::RGB24,
        data::Camera::PixelFormat::RGB565,
        data::Camera::PixelFormat::RGB555,
        data::Camera::PixelFormat::ARGB8565_PREMULTIPLIED,
        data::Camera::PixelFormat::BGRA32,
        data::Camera::PixelFormat::BGRA32_PREMULTIPLIED,
        data::Camera::PixelFormat::BGR32,
        data::Camera::PixelFormat::BGR24,
        data::Camera::PixelFormat::BGR565,
        data::Camera::PixelFormat::BGR555,
        data::Camera::PixelFormat::BGRA5658_PREMULTIPLIED,
        data::Camera::PixelFormat::AYUV444,
        data::Camera::PixelFormat::AYUV444_PREMULTIPLIED,
        data::Camera::PixelFormat::YUV444,
        data::Camera::PixelFormat::YUV420P,
        data::Camera::PixelFormat::YV12,
        data::Camera::PixelFormat::UYVY,
        data::Camera::PixelFormat::YUYV,
        data::Camera::PixelFormat::NV12,
        data::Camera::PixelFormat::NV21,
        data::Camera::PixelFormat::IMC1,
        data::Camera::PixelFormat::IMC2,
        data::Camera::PixelFormat::IMC3,
        data::Camera::PixelFormat::IMC4,
        data::Camera::PixelFormat::Y8,
        data::Camera::PixelFormat::Y16,
        data::Camera::PixelFormat::JPEG,
        data::Camera::PixelFormat::CAMERARAW,
        data::Camera::PixelFormat::ADOBEDNG,
        data::Camera::PixelFormat::RGBA32,
        data::Camera::PixelFormat::USER,
        data::Camera::PixelFormat::INVALID
    };
    object->setPixelFormat(pixelFormats[variant % 35]);
    object->setVideoFile("/" + UUID::generate());
    object->setStreamUrl(UUID::generate());
    object->setCameraSource(
        variant % 3 == 0
        ? data::Camera::SourceType::DEVICE
        : variant % 3 == 1
        ? data::Camera::SourceType::FILE
        : variant % 3 == 2
        ? data::Camera::SourceType::STREAM
        : data::Camera::SourceType::UNKNOWN
    );
    object->setScale(random<double>());

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::cameraTest()
{
    testCombine<data::Camera>();
}

//------------------------------------------------------------------------------

template<>
inline data::Color::sptr generate<data::Color>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::Color>();

    object->setRGBA(random<float>(), random<float>(), random<float>(), random<float>());

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::colorTest()
{
    testCombine<data::Color>();
}

//------------------------------------------------------------------------------

template<>
inline data::Landmarks::sptr generate<data::Landmarks>(const std::size_t variant)
{
    auto object = std::make_shared<data::Landmarks>();

    for(std::size_t i = 0, i_end = variant + 2 ; i < i_end ; ++i)
    {
        const std::string name = UUID::generate();

        object->addGroup(
            name,
            {random<float>(), random<float>(), random<float>(), random<float>()},
            random<float>(),
            variant % 2 == 0
            ? data::Landmarks::Shape::CUBE
            : data::Landmarks::Shape::SPHERE,
            variant % 3 == 0
        );

        for(std::size_t j = 0, j_end = variant + 2 ; j < j_end ; ++j)
        {
            object->addPoint(name, {random<double>(), random<double>(), random<double>()});
        }
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::landmarksTest()
{
    testCombine<data::Landmarks>();
}

//------------------------------------------------------------------------------

template<>
inline data::Line::sptr generate<data::Line>(const std::size_t variant)
{
    auto object = std::make_shared<data::Line>();

    object->setPosition(create<data::Point>(variant));
    object->setDirection(create<data::Point>(variant + 1));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::lineTest()
{
    testCombine<data::Line>();
}

//------------------------------------------------------------------------------

template<>
inline data::Material::sptr generate<data::Material>(const std::size_t variant)
{
    auto object = std::make_shared<data::Material>();

    std::array shading {
        data::Material::ShadingType::AMBIENT,
        data::Material::ShadingType::FLAT,
        data::Material::ShadingType::PHONG
    };

    std::array representation {
        data::Material::RepresentationType::EDGE,
        data::Material::RepresentationType::POINT,
        data::Material::RepresentationType::SURFACE,
        data::Material::RepresentationType::WIREFRAME
    };

    std::array options {
        data::Material::OptionsType::CELLS_NORMALS,
        data::Material::OptionsType::NORMALS,
        data::Material::OptionsType::STANDARD
    };

    // Set ambient color
    object->setAmbient(create<data::Color>(variant));

    // Set diffuse color
    object->setDiffuse(create<data::Color>(variant + 1));

    // Set diffuse texture
    object->setDiffuseTexture(create<data::Image>(variant));

    // Others
    object->setShadingMode(shading[variant % std::size(shading)]);
    object->setRepresentationMode(representation[variant % std::size(representation)]);
    object->setOptionsMode(options[variant % std::size(options)]);
    object->setDiffuseTextureFiltering(
        variant % 3 == 0
        ? data::Material::FilteringType::LINEAR
        : data::Material::FilteringType::NEAREST
    );
    object->setDiffuseTextureWrapping(
        variant % 3 == 0
        ? data::Material::WrappingType::CLAMP
        : data::Material::WrappingType::REPEAT
    );

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::materialTest()
{
    testCombine<data::Material>();
}

//------------------------------------------------------------------------------

template<>
inline data::Matrix4::sptr generate<data::Matrix4>(const std::size_t /*unused*/)
{
    auto object = std::make_shared<data::Matrix4>();

    for(double& coefficient : *object)
    {
        coefficient = random<double>();
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::matrix4Test()
{
    testCombine<data::Matrix4>();
}

//------------------------------------------------------------------------------

template<>
inline data::Plane::sptr generate<data::Plane>(const std::size_t variant)
{
    auto object = std::make_shared<data::Plane>();

    auto& points = object->getPoints();
    for(std::size_t i = 0, end = points.size() ; i < end ; ++i)
    {
        points[i] = create<data::Point>(i + variant);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeTest()
{
    testCombine<data::Plane>();
}

//------------------------------------------------------------------------------

template<>
inline data::PlaneList::sptr generate<data::PlaneList>(const std::size_t variant)
{
    auto object = std::make_shared<data::PlaneList>();

    auto& planes = object->getPlanes();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        planes.push_back(create<data::Plane>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::planeListTest()
{
    testCombine<data::PlaneList>();
}

//------------------------------------------------------------------------------

template<>
inline data::Reconstruction::sptr generate<data::Reconstruction>(const std::size_t variant)
{
    auto object = std::make_shared<data::Reconstruction>();

    object->setIsVisible(variant % 3 == 0);
    object->setOrganName(UUID::generate());
    object->setStructureType(UUID::generate());
    object->setComputedMaskVolume(random<double>());

    // Material
    object->setMaterial(create<data::Material>(variant));

    // Image
    object->setImage(create<data::Image>(variant));

    // Mesh
    object->setMesh(create<data::Mesh>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::reconstructionTest()
{
    testCombine<data::Reconstruction>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraits::sptr generate<data::StructureTraits>(const std::size_t variant)
{
    auto object = std::make_shared<data::StructureTraits>();

    const std::array CLASSES {
        data::StructureTraits::StructureClass::ENVIRONMENT,
        data::StructureTraits::StructureClass::FUNCTIONAL,
        data::StructureTraits::StructureClass::LESION,
        data::StructureTraits::StructureClass::NO_CONSTRAINT,
        data::StructureTraits::StructureClass::ORGAN,
        data::StructureTraits::StructureClass::TOOL,
        data::StructureTraits::StructureClass::VESSEL
    };

    const std::array CATEGORIES {
        data::StructureTraits::Category::ABDOMEN,
        data::StructureTraits::Category::ARM,
        data::StructureTraits::Category::BODY,
        data::StructureTraits::Category::HEAD,
        data::StructureTraits::Category::LEG,
        data::StructureTraits::Category::LIVER_SEGMENTS,
        data::StructureTraits::Category::NECK,
        data::StructureTraits::Category::OTHER,
        data::StructureTraits::Category::PELVIS,
        data::StructureTraits::Category::THORAX
    };

    object->setType(UUID::generate());
    object->setClass(CLASSES[variant % std::size(CLASSES)]);
    object->setNativeExp(UUID::generate());
    object->setNativeGeometricExp(UUID::generate());
    object->setAttachmentType(UUID::generate());
    object->setAnatomicRegion(UUID::generate());
    object->setPropertyCategory(UUID::generate());
    object->setPropertyType(UUID::generate());

    // Categories

    // Reset categories.
    data::StructureTraits::CategoryContainer categories;

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        categories.push_back(CATEGORIES[(i + variant) % std::size(CATEGORIES)]);
    }

    object->setCategories(categories);

    // Color
    object->setColor(create<data::Color>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsTest()
{
    testCombine<data::StructureTraits>();
}

//------------------------------------------------------------------------------

template<>
inline data::StructureTraitsDictionary::sptr generate<data::StructureTraitsDictionary>(const std::size_t variant)
{
    auto object = std::make_shared<data::StructureTraitsDictionary>();

    auto organ = create<data::StructureTraits>(variant);
    organ->setClass(data::StructureTraits::ORGAN);
    organ->setAttachmentType("");

    object->addStructure(organ);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto structure            = create<data::StructureTraits>(variant + i + 1);
        const auto structureClass = structure->getClass();

        if(structureClass != data::StructureTraits::LESION
           && structureClass != data::StructureTraits::FUNCTIONAL)
        {
            structure->setAttachmentType("");
        }
        else
        {
            structure->setAttachmentType(organ->getType());
        }

        object->addStructure(structure);
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::structureTraitsDictionaryTest()
{
    testCombine<data::StructureTraitsDictionary>();
}

//------------------------------------------------------------------------------

template<>
inline data::Resection::sptr generate<data::Resection>(const std::size_t variant)
{
    auto object = std::make_shared<data::Resection>();

    object->setName(UUID::generate());
    object->setIsSafePart(variant % 2 == 0);
    object->setIsValid(variant % 3 == 0);
    object->setIsVisible(variant % 4 == 0);

    auto& inputs  = object->getInputs();
    auto& outputs = object->getOutputs();
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        inputs.push_back(create<data::Reconstruction>(variant + i));
        outputs.push_back(create<data::Reconstruction>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionTest()
{
    testCombine<data::Resection>();
}

//------------------------------------------------------------------------------

template<>
inline data::ResectionDB::sptr generate<data::ResectionDB>(const std::size_t variant)
{
    auto object = std::make_shared<data::ResectionDB>();

    object->setSafeResection(create<data::Resection>(variant));

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->addResection(create<data::Resection>(variant + i + 1));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::resectionDBTest()
{
    testCombine<data::ResectionDB>();
}

//------------------------------------------------------------------------------

template<>
inline data::TransferFunction::sptr generate<data::TransferFunction>(const std::size_t variant)
{
    auto object = std::make_shared<data::TransferFunction>();

    object->setLevel(random<double>());
    object->setWindow(random<double>());
    object->setName(UUID::generate());
    object->setBackgroundColor(
        data::TransferFunction::color_t(
            random<double>(),
            random<double>(),
            random<double>(),
            random<double>()
        )
    );

    auto tfData = object->pieces().emplace_back(std::make_shared<data::TransferFunctionPiece>());
    tfData->setInterpolationMode(
        variant % 3 == 0
        ? data::TransferFunction::InterpolationMode::LINEAR
        : data::TransferFunction::InterpolationMode::NEAREST
    );
    tfData->setClamped(variant % 4 == 0);

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        tfData->insert(
            {
                random<double>(),
                data::TransferFunction::color_t(
                    random<double>(),
                    random<double>(),
                    random<double>(),
                    random<double>()
                )
            });
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::transferFunctionTest()
{
    testCombine<data::TransferFunction>();
}

//------------------------------------------------------------------------------

template<>
inline data::DicomSeries::sptr generate<data::DicomSeries>(const std::size_t variant)
{
    data::DicomSeries::sptr dicomSeries;

    // Only load the real dicom once
    if(variant == 0)
    {
        // Setup the SeriesSet to be able to read
        auto series_set                  = std::make_shared<data::SeriesSet>();
        const std::filesystem::path path = utestData::Data::dir()
                                           / "sight/Patient/Dicom/DicomDB/86-CT-Skull";

        CPPUNIT_ASSERT_MESSAGE(
            "The dicom directory '" + path.string() + "' does not exist",
            std::filesystem::exists(path)
        );

        // Read source Dicom
        auto reader = std::make_shared<io::dicom::reader::SeriesSet>();
        reader->setObject(series_set);
        reader->set_folder(path);

        CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), series_set->size());

        dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(series_set->front());
    }
    else
    {
        // Take the first variant as basis
        dicomSeries = std::make_shared<data::DicomSeries>();
        dicomSeries->shallow_copy(getExpected<data::DicomSeries>(0));
    }

    // Randomize a bit the dicomSeries
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        dicomSeries->addSOPClassUID(UUID::generate());
        dicomSeries->addComputedTagValue(UUID::generate(), UUID::generate());
    }

    // Inherited attributes
    dicomSeries->Series::shallow_copy(getExpected<data::Series>(variant));

    return dicomSeries;
}

//------------------------------------------------------------------------------

void SessionTest::dicomSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::DicomSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ImageSeries::sptr generate<data::ImageSeries>(const std::size_t variant)
{
    auto object = std::make_shared<data::ImageSeries>();

    // Inherited attributes
    object->Image::shallow_copy(getExpected<data::Image>(variant));
    object->Series::shallow_copy(getExpected<data::Series>(variant));

    object->setContrastBolusAgent(UUID::generate());
    object->setContrastBolusRoute(UUID::generate());
    object->setContrastBolusVolume(double(variant));
    object->setContrastBolusStartTime(generateTM(variant));
    object->setContrastBolusStopTime(generateTM(variant));
    object->setContrastBolusTotalDose(double(variant));

    object->setContrastFlowRate(
        std::to_string(variant)
        + "\\"
        + std::to_string(variant + 1)
        + "\\"
        + std::to_string(variant + 2)
    );

    object->setContrastFlowDuration(
        std::to_string(variant + 4)
        + "\\"
        + std::to_string(variant + 5)
        + "\\"
        + std::to_string(variant + 6)
    );

    object->setContrastBolusIngredient(UUID::generate());
    object->setContrastBolusIngredientConcentration(double(variant));
    object->setAcquisitionDate(generateDA(variant));
    object->setAcquisitionTime(generateTM(variant));

    object->setDicomReference(create<data::DicomSeries>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::imageSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::ImageSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ModelSeries::sptr generate<data::ModelSeries>(const std::size_t variant)
{
    auto object = std::make_shared<data::ModelSeries>();

    object->setDicomReference(create<data::DicomSeries>(variant));

    std::vector<data::Reconstruction::sptr> reconstructionDB;
    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        reconstructionDB.push_back(create<data::Reconstruction>(variant + i));
    }

    object->setReconstructionDB(reconstructionDB);

    // Inherited attributes
    object->Series::shallow_copy(getExpected<data::Series>(variant));

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::modelSeriesTest()
{
    if(utest::Filter::ignoreSlowTests())
    {
        return;
    }

    testCombine<data::ModelSeries>();
}

//------------------------------------------------------------------------------

template<>
inline data::ActivitySet::sptr generate<data::ActivitySet>(const std::size_t variant)
{
    auto object = std::make_shared<data::ActivitySet>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Activity>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::activitySetTest()
{
    testCombine<data::ActivitySet>();
}

//------------------------------------------------------------------------------

template<>
inline data::CameraSet::sptr generate<data::CameraSet>(const std::size_t variant)
{
    auto object = std::make_shared<data::CameraSet>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        auto camera = create<data::Camera>(variant + i);
        auto matrix = create<data::Matrix4>(variant + i);
        object->push_back(std::make_pair(camera, matrix));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::cameraSetTest()
{
    testCombine<data::CameraSet>();
}

//------------------------------------------------------------------------------

template<>
inline data::SeriesSet::sptr generate<data::SeriesSet>(const std::size_t variant)
{
    auto object = std::make_shared<data::SeriesSet>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::seriesSetTest()
{
    testCombine<data::SeriesSet>();
}

//------------------------------------------------------------------------------

template<>
inline data::Set::sptr generate<data::Set>(const std::size_t variant)
{
    auto object = std::make_shared<data::Set>();

    for(std::size_t i = 0, end = variant + 2 ; i < end ; ++i)
    {
        object->push_back(create<data::Series>(variant + i));
    }

    return object;
}

//------------------------------------------------------------------------------

void SessionTest::setTest()
{
    testCombine<data::Set>();
}

//------------------------------------------------------------------------------

inline static void customSerialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Cast to the right type
    const auto string = Helper::safe_cast<data::String>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::String>(tree, 666);

    Helper::writeString(tree, "custom", string->getValue());
}

//------------------------------------------------------------------------------

inline static data::String::sptr customDeserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto string = Helper::cast_or_create<data::String>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::String>(tree, 0, 666);

    // Assign the value
    string->setValue(Helper::readString(tree, "custom"));

    return string;
}

//------------------------------------------------------------------------------

void SessionTest::customSerializerTest()
{
    // Create a temporary directory
    core::os::temp_dir tmpDir;
    const auto testPath = tmpDir / "customSerializerTest.zip";

    // Test serialization
    {
        // Create the data object
        auto object = create<data::String>(0);

        // Create the session writer
        auto sessionWriter = std::make_shared<io::session::SessionWriter>();
        CPPUNIT_ASSERT(sessionWriter);

        // Configure the session writer
        sessionWriter->setObject(object);
        sessionWriter->set_file(testPath);

        // Test serializer getter
        CPPUNIT_ASSERT(sessionWriter->serializer(data::String::classname()));

        // Change the session serializer by setting a new one using setCustomSerializer
        sessionWriter->setCustomSerializer(data::String::classname(), customSerialize);

        // Write the new session
        CPPUNIT_ASSERT_NO_THROW(sessionWriter->write());

        CPPUNIT_ASSERT(std::filesystem::exists(testPath));
    }

    // Test deserialization
    {
        auto sessionReader = std::make_shared<io::session::SessionReader>();
        CPPUNIT_ASSERT(sessionReader);

        // Configure the session reader
        sessionReader->set_file(testPath);

        // Read the session: it should fail since the serializer has been modified by a custom one
        CPPUNIT_ASSERT_THROW(sessionReader->read(), sight::core::exception);

        // Test deserializer getter
        CPPUNIT_ASSERT(sessionReader->deserializer(data::String::classname()));

        // Set the new customDeserializer
        sessionReader->setCustomDeserializer(data::String::classname(), customDeserialize);

        CPPUNIT_ASSERT_NO_THROW(sessionReader->read());

        // Test value
        auto object = std::dynamic_pointer_cast<data::String>(sessionReader->getObject());
        compare<data::String>(object, 0);
    }
}

} // namespace sight::io::session::ut
