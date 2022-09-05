/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "cvCommon.hpp"

#include <opencv2/core.hpp>

#include <array>

namespace sight::io::opencv::ut
{

//------------------------------------------------------------------------------

using CvFormatValue = const std::array<std::int32_t, 4>;

CvFormatValue getCvFormat<std::uint8_t>::type {CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4};
CvFormatValue getCvFormat<std::int8_t>::type {CV_8SC1, CV_8SC2, CV_8SC3, CV_8SC4};
CvFormatValue getCvFormat<std::uint16_t>::type {CV_16UC1, CV_16UC2, CV_16UC3, CV_16UC4};
CvFormatValue getCvFormat<std::int16_t>::type {CV_16SC1, CV_16SC2, CV_16SC3, CV_16SC4};
CvFormatValue getCvFormat<std::int32_t>::type {CV_32SC1, CV_32SC2, CV_32SC3, CV_32SC4};
CvFormatValue getCvFormat<float>::type {CV_32FC1, CV_32FC2, CV_32FC3, CV_32FC4};
CvFormatValue getCvFormat<double>::type {CV_64FC1, CV_64FC2, CV_64FC3, CV_64FC4};

//------------------------------------------------------------------------------

} // namespace sight::io::opencv::ut
