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

#include "data/Image.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/com/Signal.hxx>

#include <numeric>

//------------------------------------------------------------------------------

SIGHT_REGISTER_DATA(sight::data::Image);

//------------------------------------------------------------------------------

namespace sight::data
{

auto pixelFormatToNumComponents = [](Image::PixelFormat format)
                                  {
                                      static const std::array<size_t,
                                                              Image::PixelFormat::_SIZE> s_pixelFormatToNumComponents =
                                      {
                                          ~0ul,
                                          3,
                                          4,
                                          3,
                                          4,
                                          1,
                                          2
                                      };
                                      return s_pixelFormatToNumComponents[format];
                                  };

const core::com::Signals::SignalKeyType Image::s_BUFFER_MODIFIED_SIG       = "bufferModified";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_ADDED_SIG        = "landmarkAdded";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_REMOVED_SIG      = "landmarkRemoved";
const core::com::Signals::SignalKeyType Image::s_LANDMARK_DISPLAYED_SIG    = "landmarkDisplayed";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_ADDED_SIG        = "distanceAdded";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_REMOVED_SIG      = "distanceRemoved";
const core::com::Signals::SignalKeyType Image::s_DISTANCE_DISPLAYED_SIG    = "distanceDisplayed";
const core::com::Signals::SignalKeyType Image::s_SLICE_INDEX_MODIFIED_SIG  = "sliceIndexModified";
const core::com::Signals::SignalKeyType Image::s_SLICE_TYPE_MODIFIED_SIG   = "sliceTypeModified";
const core::com::Signals::SignalKeyType Image::s_VISIBILITY_MODIFIED_SIG   = "visibilityModified";
const core::com::Signals::SignalKeyType Image::s_TRANSPARENCY_MODIFIED_SIG = "transparencyModified";

//------------------------------------------------------------------------------

Image::Image(data::Object::Key) :
    m_dataArray(data::Array::New())
{
    newSignal<BufferModifiedSignalType>(s_BUFFER_MODIFIED_SIG);
    newSignal<LandmarkAddedSignalType>(s_LANDMARK_ADDED_SIG);
    newSignal<LandmarkRemovedSignalType>(s_LANDMARK_REMOVED_SIG);
    newSignal<LandmarkDisplayedSignalType>(s_LANDMARK_DISPLAYED_SIG);
    newSignal<DistanceDisplayedSignalType>(s_DISTANCE_DISPLAYED_SIG);
    newSignal<DistanceAddedSignalType>(s_DISTANCE_ADDED_SIG);
    newSignal<DistanceRemovedSignalType>(s_DISTANCE_REMOVED_SIG);
    newSignal<SliceIndexModifiedSignalType>(s_SLICE_INDEX_MODIFIED_SIG);
    newSignal<SliceTypeModifiedSignalType>(s_SLICE_TYPE_MODIFIED_SIG);
    newSignal<VisibilityModifiedSignalType>(s_VISIBILITY_MODIFIED_SIG);
    newSignal<TransparencyModifiedSignalType>(s_TRANSPARENCY_MODIFIED_SIG);
}

//------------------------------------------------------------------------------

Image::~Image() noexcept
{
}

//-----------------------------------------------------------------------------

void Image::shallowCopy(const Object::csptr& _source)
{
    Image::csptr other = Image::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);

    // Assign
    copyInformation(other);

    m_dataArray = other->m_dataArray;
}

//-----------------------------------------------------------------------------

void Image::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Image::csptr other = Image::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !other
    );
    this->fieldDeepCopy(_source, cache);

    // Assign
    this->copyInformation(other);

    if(other->m_dataArray)
    {
        m_dataArray->cachedDeepCopy(other->m_dataArray, cache);
    }
}

//------------------------------------------------------------------------------

size_t Image::resize(const Size& size, const core::tools::Type& type, PixelFormat format)
{
    return this->_resize(size, type, format, true);
}

//------------------------------------------------------------------------------

size_t Image::_resize(const Size& size, const core::tools::Type& type, PixelFormat format, bool realloc)
{
    m_size          = size;
    m_type          = type;
    m_pixelFormat   = format;
    m_numComponents = pixelFormatToNumComponents(format);

    SIGHT_ASSERT("Number of components must be > 0", m_numComponents > 0);
    SIGHT_ASSERT("Number of components must be <= 4", m_numComponents <= 4);

    const size_t imageDims = this->numDimensions();
    data::Array::SizeType arraySize(imageDims);

    size_t count = 0;
    if(m_numComponents > 1)
    {
        arraySize.resize(imageDims + 1);
        arraySize[0] = m_numComponents;
        count        = 1;
    }

    for(size_t i = 0 ; i < imageDims ; ++i)
    {
        arraySize[count] = m_size[i];
        ++count;
    }

    return m_dataArray->resize(arraySize, m_type, realloc);
}

//------------------------------------------------------------------------------

core::tools::Type Image::getType() const
{
    return m_type;
}

//------------------------------------------------------------------------------

void Image::copyInformation(Image::csptr _source)
{
    m_size          = _source->m_size;
    m_spacing       = _source->m_spacing;
    m_origin        = _source->m_origin;
    m_type          = _source->m_type;
    m_windowCenter  = _source->m_windowCenter;
    m_windowWidth   = _source->m_windowWidth;
    m_numComponents = _source->m_numComponents;
    m_pixelFormat   = _source->m_pixelFormat;
}

//------------------------------------------------------------------------------

size_t Image::numDimensions() const
{
    size_t dims = 0;

    for(const auto& val : m_size)
    {
        if(val > 0)
        {
            ++dims;
        }
        else
        {
            break;
        }
    }

    return dims;
}

//------------------------------------------------------------------------------

size_t Image::getSizeInBytes() const
{
    size_t size       = 0;
    const size_t dims = this->numDimensions();
    if(dims > 0)
    {
        size = std::accumulate(
            m_size.begin(),
            m_size.begin() + dims,
            static_cast<size_t>(m_type.sizeOf()) * m_numComponents,
            std::multiplies<size_t>()
        );
    }

    return size;
}

//------------------------------------------------------------------------------

size_t Image::getAllocatedSizeInBytes() const
{
    size_t size = 0;
    if(m_dataArray)
    {
        size = m_dataArray->getSizeInBytes();
    }

    return size;
}

//------------------------------------------------------------------------------

core::memory::BufferObject::Lock Image::lock() const
{
    return m_dataArray->lock();
}

//------------------------------------------------------------------------------

void Image::lockBuffer(std::vector<core::memory::BufferObject::Lock>& locks) const
{
    locks.push_back(this->lock());
}

//-----------------------------------------------------------------------------

void* Image::getBuffer()
{
    return m_dataArray->getBuffer();
}

//-----------------------------------------------------------------------------

void* Image::getBuffer() const
{
    return m_dataArray->getBuffer();
}

//------------------------------------------------------------------------------

void* Image::getPixel(IndexType index)
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numComponents;
    BufferType* buf             = static_cast<BufferType*>(this->getBuffer());
    const IndexType bufIndex    = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void* Image::getPixel(IndexType index) const
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numComponents;
    BufferType* buf             = static_cast<BufferType*>(this->getBuffer());
    const IndexType bufIndex    = index * imagePixelSize;
    return buf + bufIndex;
}

//------------------------------------------------------------------------------

void Image::setPixel(IndexType index, Image::BufferType* pixBuf)
{
    const size_t imagePixelSize = m_type.sizeOf() * m_numComponents;
    BufferType* buf             = static_cast<BufferType*>(this->getPixel(index));

    std::copy(pixBuf, pixBuf + imagePixelSize, buf);
}

//------------------------------------------------------------------------------

const std::string Image::getPixelAsString(
    IndexType x,
    IndexType y,
    IndexType z
) const
{
    const IndexType offset = x + m_size[0] * y + z * m_size[0] * m_size[1];
    return m_type.toString(this->getPixel(offset));
}

//------------------------------------------------------------------------------

Image::iterator<char> Image::begin()
{
    return iterator<char>(static_cast<char*>(getBuffer()));
}

//------------------------------------------------------------------------------

Image::iterator<char> Image::end()
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

Image::const_iterator<char> Image::begin() const
{
    return const_iterator<char>(static_cast<const char*>(getBuffer()));
}

//------------------------------------------------------------------------------

Image::const_iterator<char> Image::end() const
{
    auto itr = begin<char>();
    itr += static_cast<typename iterator<char>::difference_type>(this->getSizeInBytes());
    return itr;
}

//------------------------------------------------------------------------------

size_t Image::numElements() const
{
    size_t nbElts = 0;
    if(m_size[0] > 0)
    {
        nbElts = m_numComponents;
        for(const auto& val : m_size)
        {
            if(val > 0)
            {
                nbElts *= val;
            }
            else
            {
                break;
            }
        }
    }

    return nbElts;
}

//------------------------------------------------------------------------------

void Image::setBuffer(
    void* buf,
    bool takeOwnership,
    const core::tools::Type& type,
    const data::Image::Size& size,
    PixelFormat format,
    core::memory::BufferAllocationPolicy::sptr policy
)
{
    this->_resize(size, type, format, false);
    this->setBuffer(buf, takeOwnership, policy);
}

//------------------------------------------------------------------------------

void Image::setBuffer(void* buf, bool takeOwnership, core::memory::BufferAllocationPolicy::sptr policy)
{
    if(m_dataArray->getIsBufferOwner())
    {
        if(!m_dataArray->getBufferObject()->isEmpty())
        {
            m_dataArray->getBufferObject()->destroy();
        }
    }
    else
    {
        core::memory::BufferObject::sptr newBufferObject = core::memory::BufferObject::New();
        core::memory::BufferObject::sptr oldBufferObject = m_dataArray->getBufferObject();
        oldBufferObject->swap(newBufferObject);
    }

    m_dataArray->getBufferObject()->setBuffer(buf, (buf == NULL) ? 0 : m_dataArray->getSizeInBytes(), policy);
    m_dataArray->setIsBufferOwner(takeOwnership);
}

//------------------------------------------------------------------------------

core::memory::BufferObject::sptr Image::getBufferObject()
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

core::memory::BufferObject::csptr Image::getBufferObject() const
{
    return m_dataArray->getBufferObject();
}

//------------------------------------------------------------------------------

} // namespace sight::data
