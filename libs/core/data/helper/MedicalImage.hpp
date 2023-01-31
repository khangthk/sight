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

#pragma once

#include "data/config.hpp"

#include <core/tools/Dispatcher.hpp>
#include <core/tools/NumericRoundCast.hxx>

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/PointList.hpp>
#include <data/thread/RegionThreader.hpp>
#include <data/TransferFunction.hpp>
#include <data/Vector.hpp>

#include <optional>

namespace sight::data::helper::MedicalImage
{

typedef enum
{
    /// Directions.
    X_AXIS = 0,
    Y_AXIS,
    Z_AXIS,
    /// Planar definitions.
    SAGITTAL = X_AXIS,
    FRONTAL  = Y_AXIS,
    AXIAL    = Z_AXIS
} orientation_t;

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _pImg   image checked.
 * @return      true if the image is valid.
 */
DATA_API bool checkImageValidity(data::Image::csptr _pImg);

/**
 * @brief       Check if the image is valid.
 *
 * The image size must be > 1 for each dimension.
 *
 * @param[in]   _pImg   image checked.
 * @return      true if the image is valid.
 */
DATA_API bool checkImageValidity(const data::Image& _image);

/**
 * @brief       Check if the image has slice index fields.
 *
 * If the image hasn't frontal, axial and sagittal slice index fields, they are added.
 *
 * @param       _pImg   image which must contains the field.
 * @return      true if the fields are added.
 */
DATA_API bool checkImageSliceIndex(data::Image::sptr _pImg);

/**
 * @brief Checks if buf contains zero, from begin to begin + len
 *
 * @param buf data::Image::BufferType* begin of the buffer.
 * @param len unsigned int length, as begin+len.
 * @return boolean, true if null, false otherwise.
 */
DATA_API bool isBufNull(const data::Image::BufferType* buf, unsigned int len);

/**
 * @brief Return a buffer of image type's size, containing 'value' casted to image data type
 * @param[in] image : reference image
 * @param[in] value : value to map
 */
template<typename T>
SPTR(data::Image::BufferType) getPixelInImageSpace(data::Image::sptr image, T & value);

/**
 * @brief Return minimum and maximum values contained in image. If image
 * min or max value is out of MINMAXTYPE range, they are clamped to
 * MINMAXTYPE capacity
 * @param[in] _img : image
 * @param[out] _min : minimum value
 * @param[out] _max : maximum value
 */
template<typename MINMAXTYPE>
void getMinMax(data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max);

// Getter/Setter for specific image fields

/**
 * @brief Helper function to get current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation
 * @return the current index as a std::int64_t.
 */
DATA_API std::optional<std::int64_t> getSliceIndex(const data::Image& _image, const orientation_t& _orientation);

/**
 * @brief Helper function to set current slice index on a medical image in a specific orientation (Axial, Sagittal,
 * Frontal).
 *
 * @param _image : input image reference
 * @param _orientation : desired orientation.
 * @param _sliceCount : current slice index to set as std::int64_t.
 */
DATA_API void setSliceIndex(
    data::Image& _image,
    const orientation_t& _orientation,
    std::int64_t _sliceCount
);

/**
 * @brief Helper function to get landmarks of a medical image.
 *
 * @param _image : input image reference.
 * @return a data::PointList::sptr containing landmarks, pointer can be null.
 */
DATA_API data::PointList::sptr getLandmarks(const data::Image& _image);

/**
 * @brief Helper function to set landmarks in a medical image.
 *
 * @param _image : input image reference.
 * @param _landmarks : landmarks pointer as data::PointList::sptr.
 */
DATA_API void setLandmarks(data::Image& _image, const data::PointList::sptr& _landmarks);

/**
 * @brief Helper function to get distances vector of a medical Image.
 *
 * @param _image : input image reference.
 * @return a data::Vector::sptr containing distances, pointer can be null.
 */
DATA_API data::Vector::sptr getDistances(const data::Image& _image);

/**
 * @brief Helper function to set distances in a medical image as a data::Vector::sptr.
 *
 * @param _image : input image reference.
 * @param _distances : data::Vector::sptr containing distances.
 */
DATA_API void setDistances(data::Image& _image, const data::Vector::sptr& _distances);

/**
 * @brief Helper function to get distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
DATA_API bool getDistanceVisibility(const data::Image& _image);

/**
 * @brief Helper function to set distance visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if distance is visible or not.
 */
DATA_API void setDistanceVisibility(data::Image& _image, bool _visibility);

/**
 * @brief Helper function to get landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @return boolean value (visible or not).
 */
DATA_API bool getLandmarksVisibility(const data::Image& _image);

/**
 * @brief Helper function to set landmarks visibility on a medical image.
 *
 * @param _image : input image reference.
 * @param _visibility : boolean to represents if landmarks are visible or not.
 */
DATA_API void setLandmarksVisibility(data::Image& _image, bool _visibility);

/**
 * @brief Helper to get comment on medical image.
 *
 * @param _image : input image reference.
 * @return std::string the comment, can be empty.
 */
DATA_API std::string getComment(const data::Image& _image);

/**
 * @brief Helper function to set comment on medical image.
 *
 * @param _image : input image reference.
 * @param _comment : std::string comment to add.
 */
DATA_API void setComment(data::Image& _image, const std::string& _comment);

/**
 * @brief Helper function to get label on a medical image.
 *
 * @param _image : input image reference.
 * @return std::string the label, can be empty.
 */
DATA_API std::string getLabel(const data::Image& _image);

/**
 * @brief Helper function to set label on medical image.
 *
 * @param _image : input image reference.
 * @param _label : std::string the label.
 */
DATA_API void setLabel(data::Image& _image, const std::string& _label);

// ------------------------------------------------------------------------------

template<typename VALUE>
class PixelCastAndSetFunctor
{
public:

    class Param
    {
    public:

        typedef VALUE ValueType;
        typedef SPTR(data::Image::BufferType) BufferTypeSptr;

        Param(ValueType& v) :
            value(v)
        {
        }

        const ValueType& value;
        BufferTypeSptr res;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        unsigned char imageTypeSize = sizeof(IMAGE);

        auto val = core::tools::numericRoundCast<IMAGE>(param.value);

        auto* buf = reinterpret_cast<data::Image::BufferType*>(&val);

        SPTR(data::Image::BufferType) res(new data::Image::BufferType[imageTypeSize]);
        std::copy(buf, buf + imageTypeSize, res.get());
        param.res = res;
    }
};

// ------------------------------------------------------------------------------

template<typename VALUE, typename INT_INDEX>
class CastAndSetFunctor
{
public:

    class Param
    {
    public:

        typedef VALUE ValueType;
        typedef INT_INDEX PointType;

        Param(PointType& p, ValueType& v) :
            value(v),
            point(p)
        {
        }

        data::Image::sptr image;
        const ValueType& value;
        const PointType& point;
    };

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        auto* buffer                  = static_cast<IMAGE*>(param.image->getBuffer());
        const INT_INDEX& p            = param.point;
        const data::Image::Size& size = param.image->getSize();
        const int& sx                 = size[0];
        const int& sy                 = size[1];
        const int& offset             = p[0] + sx * p[1] + p[2] * sx * sy;
        *(buffer + offset) = core::tools::numericRoundCast<IMAGE>(param.value);
    }
};

// ------------------------------------------------------------------------------

template<typename T>
SPTR(data::Image::BufferType) getPixelInImageSpace(
    data::Image::sptr image,
    T & value
)
{
    typename PixelCastAndSetFunctor<T>::Param param(value);

    core::Type type = image->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, PixelCastAndSetFunctor<T> >::invoke(type, param);
    return param.res;
}

// ------------------------------------------------------------------------------

template<typename T>
class MinMaxFunctor
{
public:

    class Param
    {
    public:

        Param(data::Image::csptr _img, T& _min, T& _max) :
            image(_img),
            min(_min),
            max(_max)
        {
        }

        data::Image::csptr image;
        T& min;
        T& max;
    };

    using result_vector_t = std::vector<T>;

    //------------------------------------------------------------------------------

    template<typename IMAGE>
    static void getMinMax(
        const data::Image::const_iterator<IMAGE>& imgBegin,
        result_vector_t& minRes,
        result_vector_t& maxRes,
        std::ptrdiff_t regionMin,
        std::ptrdiff_t regionMax,
        std::size_t i
)
    {
        const data::Image::const_iterator<IMAGE> begin = imgBegin + regionMin;
        const data::Image::const_iterator<IMAGE> end   = imgBegin + regionMax;

        typedef std::numeric_limits<IMAGE> ImgLimits;
        IMAGE imin = ImgLimits::max();
        IMAGE imax = ImgLimits::lowest();

        for(auto itr = begin ; itr != end ; ++itr)
        {
            IMAGE currentVoxel = *itr;

            if(currentVoxel < imin)
            {
                imin = currentVoxel;
            }
            else if(currentVoxel > imax)
            {
                imax = currentVoxel;
            }
        }

        typedef std::numeric_limits<T> TLimits;
        constexpr T minT = TLimits::lowest();
        constexpr T maxT = TLimits::max();

        const T min = (static_cast<T>(imin) < minT) ? minT : static_cast<T>(imin);
        const T max = (static_cast<T>(imax) > maxT) ? maxT : static_cast<T>(imax);

        minRes[i] = min;
        maxRes[i] = max;
    }

    // ------------------------------------------------------------------------------

    template<typename IMAGE>
    void operator()(Param& param)
    {
        const data::Image::csptr image = param.image;
        const auto dumpLock            = image->dump_lock();

        result_vector_t min_result;
        result_vector_t max_result;

        sight::data::thread::RegionThreader rt;
        min_result.resize(rt.numberOfThread());
        max_result.resize(rt.numberOfThread());
        rt(
            [capture0 = image->cbegin<IMAGE>(), &min_result, &max_result](std::ptrdiff_t PH1, std::ptrdiff_t PH2,
                                                                          std::size_t PH3, auto&& ...)
            {
                return MinMaxFunctor::getMinMax<IMAGE>(
                    capture0,
                    min_result,
                    max_result,
                    PH1,
                    PH2,
                    PH3
                );
            },
            image->cend<IMAGE>() - image->cbegin<IMAGE>()
        );

        param.min = *std::min_element(min_result.begin(), min_result.end());
        param.max = *std::max_element(max_result.begin(), max_result.end());
    }
};

// ------------------------------------------------------------------------------

template<typename MINMAXTYPE>
void getMinMax(const data::Image::csptr _img, MINMAXTYPE& _min, MINMAXTYPE& _max)
{
    typename MinMaxFunctor<MINMAXTYPE>::Param param(_img, _min, _max);

    core::Type type = _img->getType();
    core::tools::Dispatcher<core::tools::SupportedDispatcherTypes, MinMaxFunctor<MINMAXTYPE> >::invoke(type, param);
}

} // namespace sight::data::helper::MedicalImage
