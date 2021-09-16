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

#pragma once

#include "utestData/config.hpp"

#include <core/base.hpp>
#include <core/tools/Type.hpp>

#include <data/Image.hpp>

namespace sight::utestData
{

namespace generator
{

/**
 * @brief   This class contains helper to generate images.
 */
class Image
{
public:

    /**
     * @brief Generate an image with the given informations. Buffer is filled with 0.
     * @param image     image to generate
     * @param size      vector of image size
     * @param spacing   vector of image spacing
     * @param origin    vector of image origin
     * @param type      image type
     * @param format    image format
     */
    UTESTDATA_API static void generateImage(
        data::Image::sptr image,
        data::Image::Size size,
        data::Image::Spacing spacing,
        data::Image::Origin origin,
        core::tools::Type type,
        data::Image::PixelFormat format
    );

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    UTESTDATA_API static void generateRandomImage(data::Image::sptr image, core::tools::Type type);

    /// Fill image array with random value
    UTESTDATA_API static void randomizeImage(data::Image::sptr image);

    /// Fill array with random value
    UTESTDATA_API static void randomizeArray(data::Array::sptr array);

    /// Creates an Array with the given type and size and fills buffer with random values.
    UTESTDATA_API static data::Array::sptr createRandomizedArray(
        const std::string& type,
        data::Array::SizeType sizes
    );
};

} // namespace generator

} // namespace sight::utestData