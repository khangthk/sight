/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "core/memory/BufferObject.hpp"

namespace sight::core::memory
{

BufferObject::BufferObject(bool autoDelete) :
    m_bufferManager(core::memory::BufferManager::getDefault()),
    m_allocPolicy(core::memory::BufferNoAllocPolicy::New()),
    m_autoDelete(autoDelete)
{
    m_bufferManager->registerBuffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

BufferObject::~BufferObject()
{
    // If the buffer is not owned like in sight::data::Array, we must destroy it by ourselves
    ///@todo remove this when sight::data::DicomSeries will be removed
    if(m_autoDelete)
    {
        m_bufferManager->destroyBuffer(&m_buffer).get();
    }

    // In the past we asserted that m_count was expired, but it can not be ensured because the unlock is asynchronous
    // So we simply unregister the buffer and we will check the counter value on the buffer manager thread instead
    m_bufferManager->unregisterBuffer(&m_buffer).get();
}

//------------------------------------------------------------------------------

void BufferObject::allocate(SizeType size, const core::memory::BufferAllocationPolicy::sptr& policy)
{
    m_bufferManager->allocateBuffer(&m_buffer, size, policy).get();
    m_allocPolicy = policy;
    m_size        = size;
}

//------------------------------------------------------------------------------

void BufferObject::reallocate(SizeType size)
{
    m_bufferManager->reallocateBuffer(&m_buffer, size).get();
    m_size = size;
}

//------------------------------------------------------------------------------

void BufferObject::destroy()
{
    m_bufferManager->destroyBuffer(&m_buffer).get();
    m_allocPolicy = core::memory::BufferNoAllocPolicy::New();
    m_size        = 0;
}

//------------------------------------------------------------------------------

void BufferObject::setBuffer(
    core::memory::BufferManager::BufferType buffer,
    SizeType size,
    const core::memory::BufferAllocationPolicy::sptr& policy,
    bool autoDelete
)
{
    m_bufferManager->setBuffer(&m_buffer, buffer, size, policy).get();
    m_allocPolicy = policy;
    m_size        = size;
    m_autoDelete  = autoDelete;
}

//------------------------------------------------------------------------------

BufferObject::Lock BufferObject::lock()
{
    return {this->getSptr()};
}

//------------------------------------------------------------------------------

BufferObject::ConstLock BufferObject::lock() const
{
    return {this->getConstSptr()};
}

//------------------------------------------------------------------------------

void BufferObject::swap(const BufferObject::sptr& _source)
{
    m_bufferManager->swapBuffer(&m_buffer, &(_source->m_buffer)).get();

    std::swap(m_size, _source->m_size);
    m_bufferManager.swap(_source->m_bufferManager);
    m_allocPolicy.swap(_source->m_allocPolicy);
}

//------------------------------------------------------------------------------

BufferManager::StreamInfo BufferObject::getStreamInfo() const
{
    return m_bufferManager->getStreamInfo(&m_buffer).get();
}

//------------------------------------------------------------------------------

void BufferObject::setIStreamFactory(
    const SPTR(core::memory::stream::in::IFactory)& factory,
    SizeType size,
    const std::filesystem::path& sourceFile,
    core::memory::FileFormatType format,
    const core::memory::BufferAllocationPolicy::sptr& policy
)
{
    m_size        = size;
    m_allocPolicy = policy;
    m_bufferManager->setIStreamFactory(&m_buffer, factory, size, sourceFile, format, policy).get();
}

//------------------------------------------------------------------------------

bool BufferObject::operator==(const BufferObject& other) const noexcept
{
    return m_buffer == other.m_buffer
           || (m_size == other.m_size && std::memcmp(m_buffer, other.m_buffer, m_size) == 0);
}

//------------------------------------------------------------------------------

bool BufferObject::operator!=(const BufferObject& other) const noexcept
{
    return !(*this == other);
}

} //namespace sight::core::memory
