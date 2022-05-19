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

#include "io/igtl/detail/converter/ImageConverter.hpp"

#include "io/igtl/detail/DataConverter.hpp"
#include "io/igtl/detail/ImageTypeConverter.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <igtlImageMessage.h>

#include <algorithm>

namespace sight::io::igtl::detail
{

namespace converter
{

const std::string ImageConverter::s_IGTL_TYPE          = "IMAGE";
const std::string ImageConverter::s_FWDATA_OBJECT_TYPE = data::Image::classname();

converterRegisterMacro(io::igtl::detail::converter::ImageConverter);

ImageConverter::ImageConverter()
{
}

//-----------------------------------------------------------------------------

ImageConverter::~ImageConverter()
{
}

//-----------------------------------------------------------------------------

::igtl::MessageBase::Pointer ImageConverter::fromFwDataObject(data::Object::csptr src) const
{
    data::Image::csptr srcImg = data::Image::dynamicCast(src);
    ::igtl::Matrix4x4 matrix;

    const auto dumpLock = srcImg->dump_lock();
    auto itr            = srcImg->begin();
    auto itrEnd         = srcImg->end();

    ::igtl::ImageMessage::Pointer dest = ::igtl::ImageMessage::New();
    ::igtl::IdentityMatrix(matrix);
    dest->SetMatrix(matrix);
    dest->SetScalarType(ImageTypeConverter::getIgtlType(srcImg->getType()));
    dest->SetCoordinateSystem(::igtl::ImageMessage::COORDINATE_LPS);
    dest->SetOrigin(float(srcImg->getOrigin()[0]), float(srcImg->getOrigin()[1]), float(srcImg->getOrigin()[2]));
    dest->SetSpacing(float(srcImg->getSpacing()[0]), float(srcImg->getSpacing()[1]), float(srcImg->getSpacing()[2]));
    dest->SetNumComponents(static_cast<int>(srcImg->numComponents()));
    dest->SetDimensions(
        static_cast<int>(srcImg->getSize()[0]),
        static_cast<int>(srcImg->getSize()[1]),
        static_cast<int>(srcImg->getSize()[2])
    );
    dest->AllocateScalars();
    char* igtlImgBuffer = reinterpret_cast<char*>(dest->GetScalarPointer());
    std::copy(itr, itrEnd, igtlImgBuffer);
    return ::igtl::MessageBase::Pointer(dest.GetPointer());
}

//-----------------------------------------------------------------------------

data::Object::sptr ImageConverter::fromIgtlMessage(const ::igtl::MessageBase::Pointer src) const
{
    ::igtl::ImageMessage::Pointer srcImg;
    char* igtlImageBuffer;
    data::Image::sptr destImg = data::Image::New();
    const auto dumpLock       = destImg->dump_lock();
    float igtlSpacing[3];
    float igtlOrigins[3];
    int igtlDimensions[3];
    data::Image::Spacing spacing;
    data::Image::Origin origins;
    data::Image::Size size;

    srcImg = ::igtl::ImageMessage::Pointer(dynamic_cast< ::igtl::ImageMessage*>(src.GetPointer()));
    srcImg->GetSpacing(igtlSpacing);
    srcImg->GetOrigin(igtlOrigins);
    srcImg->GetDimensions(igtlDimensions);
    std::transform(igtlSpacing, igtlSpacing + 3, spacing.begin(), boost::numeric_cast<double, float>);
    std::copy(igtlDimensions, igtlDimensions + 3, size.begin());
    std::transform(igtlOrigins, igtlOrigins + 3, origins.begin(), boost::numeric_cast<double, float>);
    destImg->setOrigin(origins);
    destImg->setSpacing(spacing);

    sight::data::Image::PixelFormat format = data::Image::PixelFormat::GRAY_SCALE;
    if(srcImg->GetNumComponents() == 1)
    {
        format = data::Image::PixelFormat::GRAY_SCALE;
    }
    else if(srcImg->GetNumComponents() == 3)
    {
        format = data::Image::PixelFormat::RGB;
    }
    else if(srcImg->GetNumComponents() == 4)
    {
        format = data::Image::PixelFormat::RGBA;
    }
    else
    {
        SIGHT_ASSERT("invalid number of components: " + std::to_string(srcImg->GetNumComponents()), false);
    }

    destImg->resize(size, ImageTypeConverter::getFwToolsType(std::uint8_t(srcImg->GetScalarType())), format);
    auto destIter = destImg->begin();
    igtlImageBuffer = reinterpret_cast<char*>(srcImg->GetScalarPointer());
    std::copy(igtlImageBuffer, igtlImageBuffer + srcImg->GetImageSize(), destIter);

    if(sight::data::helper::MedicalImage::checkImageValidity(destImg))
    {
        sight::data::helper::MedicalImage::checkTransferFunctionPool(destImg);
        sight::data::helper::MedicalImage::checkImageSliceIndex(destImg);
    }

    return destImg;
}

//-----------------------------------------------------------------------------

IConverter::sptr ImageConverter::New()
{
    return std::make_shared<ImageConverter>();
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::getIgtlType() const
{
    return ImageConverter::s_IGTL_TYPE;
}

//-----------------------------------------------------------------------------

std::string const& ImageConverter::getFwDataObjectType() const
{
    return ImageConverter::s_FWDATA_OBJECT_TYPE;
}

} // namespace converter

} // namespace sight::io::igtl::detail}
