/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "WriterTest.hpp"

#include <core/tools/System.hpp>
#include <core/tools/UUID.hpp>

#include <data/ImageSeries.hpp>

#include <io/dicom/Reader.hpp>
#include <io/dicom/Writer.hpp>

#include <utest/Filter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <chrono>
#include <ctime>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::ut::WriterTest);

namespace sight::io::dicom::ut
{

//------------------------------------------------------------------------------

inline static std::filesystem::path createTempFolder()
{
    auto tmp_folder = core::tools::System::getTemporaryFolder() / core::tools::UUID::generateUUID();
    std::filesystem::remove_all(tmp_folder);
    std::filesystem::create_directories(tmp_folder);

    return tmp_folder;
}

//------------------------------------------------------------------------------

/// @note this function is only needed because std::format (c++20) is not supported by clang 14 / gcc 12
/// It is bold C API to format a date / time to something like "YYYYMMDDHHMMSS.FFFFFF" (DICOM DateTime format)
inline static std::string formatDateTime(const std::chrono::time_point<std::chrono::system_clock>& now)
{
    time_t now_time = std::chrono::system_clock::to_time_t(now);

    struct tm timeinfo {};

#ifdef WIN32
    localtime_s(&timeinfo, &now_time);
#else
    localtime_r(&now_time, &timeinfo);
#endif

    const auto truncated_now = std::chrono::system_clock::from_time_t(now_time);
    const auto ms            = (now - truncated_now).count();

    std::string buffer("YYYYMMDDHHMMSS.");
    const auto size = strftime(buffer.data(), buffer.size(), "%Y%m%d%H%M%S", &timeinfo);
    buffer.resize(size);

    return buffer + "." + std::to_string(ms).substr(0, 6);
}

//------------------------------------------------------------------------------

inline static data::ImageSeries::sptr getUSVolumeImage(
    std::uint32_t seed              = 0,
    std::size_t num_frames          = 1,
    core::Type type                 = core::Type::UINT8,
    data::Image::PixelFormat format = data::Image::RGB
)
{
    using key_t = std::tuple<std::size_t, core::Type, data::Image::PixelFormat, std::uint32_t>;
    static std::map<key_t, data::ImageSeries::sptr> generated;

    const key_t key {num_frames, type, format, seed};

    const auto& it = generated.find(key);

    if(it == generated.end())
    {
        auto image_series = data::ImageSeries::New();

        utestData::generator::Image::generateImage(
            image_series,
            {64, 64, num_frames},
            {1.0, 1.0, 1.0},
            {0, 0, 0},
            type,
            format
        );

        utestData::generator::Image::randomizeImage(image_series, seed);

        // We want an Enhanced US Volume
        image_series->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);

        // Set Image Position Patient / Image Orientation Patient
        for(std::size_t frame_index = 0 ; frame_index < num_frames ; ++frame_index)
        {
            // ..Image Position / Orientation Patient is what we want
            image_series->setImagePositionPatient(
                {
                    double(seed + 1) * 0.1,
                    double(seed + 1) * 0.2,
                    double(seed + 1) * 0.3
                },
                frame_index
            );

            image_series->setImageOrientationPatient(
                {
                    double(seed + 1) * 0.4,
                    double(seed + 1) * 0.5,
                    double(seed + 1) * 0.6,
                    double(seed + 1) * 0.7,
                    double(seed + 1) * 0.8,
                    double(seed + 1) * 0.9,
                },
                frame_index
            );

            // set the Frame Acquisition Date Time, which is our "timestamp"
            auto now = std::chrono::system_clock::now();
            now += std::chrono::milliseconds(frame_index);

            //YYYYMMDDHHMMSS.FFFFFF
            image_series->setFrameAcquisitionDateTime(formatDateTime(now), frame_index);
        }

        generated[key] = image_series;

        return image_series;
    }

    return it->second;
}

//------------------------------------------------------------------------------

inline static void compareEnhancedUSVolume(
    const data::ImageSeries::sptr& expected,
    const data::ImageSeries::sptr& actual
)
{
    CPPUNIT_ASSERT(expected);
    CPPUNIT_ASSERT(actual);

    // SOP Class UID
    CPPUNIT_ASSERT_EQUAL(expected->getSOPKeyword(), actual->getSOPKeyword());

    // Sizes
    const auto& expected_sizes = expected->getSize();
    const auto& actual_sizes   = actual->getSize();
    CPPUNIT_ASSERT_EQUAL(expected_sizes.size(), actual_sizes.size());

    for(std::size_t i = 0 ; i < expected_sizes.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(expected_sizes[i], actual_sizes[i]);
    }

    // Type
    CPPUNIT_ASSERT_EQUAL(expected->getType(), actual->getType());

    // Pixel format
    CPPUNIT_ASSERT_EQUAL(expected->getPixelFormat(), actual->getPixelFormat());

    // Spacings
    const auto& expected_spacing = expected->getSpacing();
    const auto& actual_spacing   = actual->getSpacing();
    CPPUNIT_ASSERT_EQUAL(expected_spacing.size(), actual_spacing.size());

    for(std::size_t i = 0 ; i < expected_spacing.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_spacing[i], actual_spacing[i], 0.0001);
    }

    // Origins
    const auto& expected_origin = expected->getOrigin();
    const auto& actual_origin   = actual->getOrigin();
    CPPUNIT_ASSERT_EQUAL(expected_origin.size(), actual_origin.size());

    for(std::size_t i = 0 ; i < expected_origin.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_origin[i], actual_origin[i], 0.0001);
    }

    // Get Image Position Patient / Image Orientation Patient
    for(std::size_t frame_index = 0 ; frame_index < actual_sizes[2] ; ++frame_index)
    {
        // Image Position Patient
        const auto& expected_position = expected->getImagePositionPatient(frame_index);
        const auto& actual_position   = actual->getImagePositionPatient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_position.size(), actual_position.size());

        for(std::size_t i = 0 ; i < expected_position.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_position[i], actual_position[i], 0.0001);
        }

        // Image Orientation Patient
        const auto& expected_orientation = expected->getImageOrientationPatient(frame_index);
        const auto& actual_orientation   = actual->getImageOrientationPatient(frame_index);
        CPPUNIT_ASSERT_EQUAL(expected_orientation.size(), actual_orientation.size());

        for(std::size_t i = 0 ; i < expected_orientation.size() ; ++i)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expected_orientation[i], actual_orientation[i], 0.0001);
        }

        // FrameAcquisitionDateTime
        CPPUNIT_ASSERT_EQUAL(
            *expected->getFrameAcquisitionDateTime(frame_index),
            *actual->getFrameAcquisitionDateTime(frame_index)
        );
    }

    // Ensure that getting value outside the frame range returns std::nullopts
    CPPUNIT_ASSERT(
        !expected->getFrameAcquisitionDateTime(actual_sizes[2])
        && !actual->getFrameAcquisitionDateTime(actual_sizes[2])
    );

    // Compare buffer
    const auto expected_locked = expected->dump_lock();
    const auto actual_locked   = actual->dump_lock();
    CPPUNIT_ASSERT_EQUAL(0, std::memcmp(expected->getBuffer(), actual->getBuffer(), expected->getSizeInBytes()));
}

//------------------------------------------------------------------------------

void WriterTest::setUp()
{
    // Set up context before running a test.
    core::memory::BufferManager::getDefault()->setLoadingMode(core::memory::BufferManager::DIRECT);
}

//------------------------------------------------------------------------------

void WriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void WriterTest::writeEnhancedUSVolumeTest()
{
    {
        const auto& folder   = createTempFolder();
        const auto& expected = getUSVolumeImage(0);

        // Write a single RGB uint8 frame image
        {
            auto seriesSet = data::SeriesSet::New();
            seriesSet->push_back(expected);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written single frame image
        {
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();
            reader->setObject(seriesSet);
            reader->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, data::ImageSeries::dynamicCast(seriesSet->front()));
        }
    }

    {
        const auto& folder   = createTempFolder();
        const auto& expected = getUSVolumeImage(1, 4);

        // Write a 4 frames RGB uint8 image
        {
            auto seriesSet = data::SeriesSet::New();
            seriesSet->push_back(expected);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();
            reader->setObject(seriesSet);
            reader->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, data::ImageSeries::dynamicCast(seriesSet->front()));
        }
    }

    {
        const auto& folder   = createTempFolder();
        const auto& expected = getUSVolumeImage(2, 4, core::Type::UINT16, data::Image::PixelFormat::GRAY_SCALE);

        // Write a 4 frames monochrome uint16 image
        {
            auto seriesSet = data::SeriesSet::New();
            seriesSet->push_back(expected);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(writer->write());
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();
            reader->setObject(seriesSet);
            reader->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, data::ImageSeries::dynamicCast(seriesSet->front()));
        }
    }

    {
        const auto& folder   = createTempFolder();
        const auto& expected = getUSVolumeImage(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = data::SeriesSet::New();
            seriesSet->push_back(expected);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);
            writer->setFile("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(folder / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();
            reader->setObject(seriesSet);
            reader->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, data::ImageSeries::dynamicCast(seriesSet->front()));
        }
    }

    {
        const auto& folder    = createTempFolder();
        const auto& expected0 = getUSVolumeImage(0, 4);
        const auto& expected1 = getUSVolumeImage(1, 4);
        const auto& expected2 = getUSVolumeImage(2, 4);

        // Write a three 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = data::SeriesSet::New();
            seriesSet->push_back(expected0);
            seriesSet->push_back(expected1);
            seriesSet->push_back(expected2);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);
            writer->setFile("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(folder / "000-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(folder / "001-custom_filename.dcm"));
            CPPUNIT_ASSERT(std::filesystem::exists(folder / "002-custom_filename.dcm"));
        }
    }

    // test resized ImageSeries
    {
        const auto& folder   = createTempFolder();
        const auto& expected = getUSVolumeImage(2, 4);

        // Write a 4 frames RGB uint8 image, with a custom filename
        {
            auto seriesSet = data::SeriesSet::New();

            auto resized = data::ImageSeries::New();
            resized->deepCopy(expected);

            // Resize and set a value at the end...
            auto new_size = expected->getSize();
            new_size[2] += 1;
            resized->resize(new_size, expected->getType(), expected->getPixelFormat());
            resized->setFrameAcquisitionDateTime(formatDateTime(std::chrono::system_clock::now()), new_size[2] - 1);

            // Resize back to original size
            resized->resize(expected->getSize(), expected->getType(), expected->getPixelFormat());
            seriesSet->push_back(resized);

            auto writer = io::dicom::Writer::New();
            writer->setObject(seriesSet);
            writer->setFolder(folder);
            writer->setFile("custom_filename.dcm");

            CPPUNIT_ASSERT_NO_THROW(writer->write());
            CPPUNIT_ASSERT(std::filesystem::exists(folder / "custom_filename.dcm"));
        }

        // Read the previously written 4 frames image
        {
            auto seriesSet = data::SeriesSet::New();
            auto reader    = io::dicom::Reader::New();
            reader->setObject(seriesSet);
            reader->setFolder(folder);

            CPPUNIT_ASSERT_NO_THROW(reader->read());
            CPPUNIT_ASSERT_EQUAL(std::size_t(1), seriesSet->size());

            compareEnhancedUSVolume(expected, data::ImageSeries::dynamicCast(seriesSet->front()));
        }
    }
}

} // namespace sight::io::dicom::ut