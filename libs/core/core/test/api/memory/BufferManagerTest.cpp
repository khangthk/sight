/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "BufferManagerTest.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/memory/BufferObject.hpp>
#include <core/memory/IPolicy.hpp>
#include <core/memory/policy/AlwaysDump.hpp>
#include <core/memory/policy/BarrierDump.hpp>
#include <core/memory/policy/NeverDump.hpp>
#include <core/memory/policy/ValveDump.hpp>

#include <utest/wait.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::memory::ut::BufferManagerTest);

namespace sight::core::memory
{

namespace ut
{

//------------------------------------------------------------------------------

void BufferManagerTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void BufferManagerTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void BufferManagerTest::allocateTest()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    const int SIZE                      = 100000;
    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(SIZE);

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    // We need to wait before checking that the buffer was unlocked because all buffer operations are done on a worker.
    // The actual buffer ref count might still be owned (as a std::promise) by the worker task when we reach this point.
    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    {
        core::memory::BufferObject::Lock lock(bo->lock());

        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(1), bo->lockCount());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    {
        core::memory::BufferObject::Lock lock(bo->lock());

        fwTestWaitMacro(bo->lockCount() == 1);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(1), bo->lockCount());
        core::memory::BufferObject::Lock lock2(bo->lock());
        fwTestWaitMacro(bo->lockCount() == 2);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(2), bo->lockCount());
        core::memory::BufferObject::csptr cbo = bo;
        core::memory::BufferObject::ConstLock clock(cbo->lock());
        fwTestWaitMacro(bo->lockCount() == 3);
        CPPUNIT_ASSERT_EQUAL(static_cast<long>(3), bo->lockCount());
    }

    fwTestWaitMacro(bo->lockCount() == 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<long>(0), bo->lockCount());

    bo->destroy();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);

    bo->allocate(SIZE, core::memory::BufferNewPolicy::New());

    CPPUNIT_ASSERT(!bo->isEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<core::memory::BufferObject::SizeType>(SIZE), bo->getSize());
    CPPUNIT_ASSERT(bo->lock().getBuffer() != NULL);

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            buf[i] = static_cast<char>(i % 256);
        }
    }

    {
        core::memory::BufferObject::Lock lock(bo->lock());
        char* buf = static_cast<char*>(lock.getBuffer());

        for(int i = 0 ; i < SIZE ; ++i)
        {
            CPPUNIT_ASSERT_EQUAL(static_cast<char>(i % 256), buf[i]);
        }
    }

    bo->destroy();

    CPPUNIT_ASSERT(bo->isEmpty());
    CPPUNIT_ASSERT(bo->lock().getBuffer() == NULL);
}

//------------------------------------------------------------------------------

void BufferManagerTest::memoryInfoTest()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    {
        SIGHT_INFO(manager->toString().get());
        core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();
        const int SIZE                      = 100000;
        bo->allocate(SIZE);
        SIGHT_INFO(manager->toString().get());
        core::memory::BufferObject::sptr bo1 = core::memory::BufferObject::New();
        SIGHT_INFO(manager->toString().get());
        {
            core::memory::BufferObject::Lock lock1(bo1->lock());
            SIGHT_INFO(manager->toString().get());
        }
        core::memory::BufferObject::sptr bo2 = core::memory::BufferObject::New();
        SIGHT_INFO(manager->toString().get());
        bo->reallocate(SIZE * 2);
        {
            core::memory::BufferObject::Lock lock(bo->lock());
            SIGHT_INFO(manager->toString().get());
        }
        bo->destroy();
        SIGHT_INFO(manager->toString().get());
        bo1->allocate(SIZE);
        bo2->allocate(SIZE);
        char* buff = new char [SIZE];
        bo->setBuffer(buff, SIZE, core::memory::BufferNewPolicy::New());
        SIGHT_INFO(manager->toString().get());

        {
            core::memory::BufferObject::Lock lock(bo->lock());
        }
        {
            core::memory::BufferObject::Lock lock(bo1->lock());
        }
        {
            core::memory::BufferObject::Lock lock(bo2->lock());
        }

        bo->destroy();
        bo1->destroy();
        bo2->destroy();
    }
    SIGHT_INFO(manager->toString().get());
}

//------------------------------------------------------------------------------

void BufferManagerTest::swapTest()
{
    core::memory::BufferManager::sptr manager     = core::memory::BufferManager::getDefault();
    core::memory::BufferObject::sptr bo1          = core::memory::BufferObject::New();
    core::memory::BufferObject::sptr bo2          = core::memory::BufferObject::New();
    core::memory::BufferAllocationPolicy::sptr p1 = core::memory::BufferMallocPolicy::New();
    core::memory::BufferAllocationPolicy::sptr p2 = core::memory::BufferNewPolicy::New();
    bo1->allocate(sizeof(int), p1);
    bo2->allocate(sizeof(long), p2);
    void* b1 = bo1->lock().getBuffer();
    void* b2 = bo2->lock().getBuffer();
    *static_cast<int*>(b1)  = 1;
    *static_cast<long*>(b2) = 2l;
    bo1->swap(bo2);
    core::memory::BufferManager::BufferInfoMapType map = manager->getBufferInfos().get();
    b1 = bo1->lock().getBuffer();
    b2 = bo2->lock().getBuffer();
    CPPUNIT_ASSERT_EQUAL(2l, *static_cast<long*>(b1));
    CPPUNIT_ASSERT_EQUAL(p2, map[bo1->getBufferPointer()].bufferPolicy);
    CPPUNIT_ASSERT_EQUAL(sizeof(long), map[bo1->getBufferPointer()].size);
    CPPUNIT_ASSERT_EQUAL(1, *static_cast<int*>(b2));
    CPPUNIT_ASSERT_EQUAL(p1, map[bo2->getBufferPointer()].bufferPolicy);
    CPPUNIT_ASSERT_EQUAL(sizeof(int), map[bo2->getBufferPointer()].size);
    bo1->destroy();
    bo2->destroy();
}

//------------------------------------------------------------------------------

void BufferManagerTest::dumpRestoreTest()
{
    core::memory::BufferManager::sptr manager        = core::memory::BufferManager::getDefault();
    core::memory::BufferObject::sptr bo              = core::memory::BufferObject::New();
    core::memory::BufferManager::BufferStats stats   = manager->getBufferStats().get();
    const core::memory::BufferManager::SizeType zero = 0;
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalManaged);
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalDumped);

    bo->allocate(sizeof(char));
    stats = manager->getBufferStats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.totalManaged);
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalDumped);
    *static_cast<char*>(bo->lock().getBuffer()) = '!';

    manager->dumpBuffer(bo->getBufferPointer()).wait();
    stats = manager->getBufferStats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.totalManaged);
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.totalDumped);

    core::memory::BufferManager::BufferInfoMapType map = manager->getBufferInfos().get();
    std::ifstream fs(map[bo->getBufferPointer()].fsFile.string());
    char x;
    fs >> x;
    CPPUNIT_ASSERT_EQUAL('!', x);

    manager->restoreBuffer(bo->getBufferPointer()).wait();
    stats = manager->getBufferStats().get();
    CPPUNIT_ASSERT_EQUAL(sizeof(char), stats.totalManaged);
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalDumped);
    CPPUNIT_ASSERT_EQUAL('!', *static_cast<char*>(bo->lock().getBuffer()));

    bo->destroy();
    stats = manager->getBufferStats().get();
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalManaged);
    CPPUNIT_ASSERT_EQUAL(zero, stats.totalDumped);
}

//------------------------------------------------------------------------------

void BufferManagerTest::dumpPolicyTest()
{
    core::memory::BufferManager::sptr manager = core::memory::BufferManager::getDefault();

    // Always Dump
    {
        core::memory::IPolicy::sptr always = core::memory::policy::AlwaysDump::New();
        CPPUNIT_ASSERT(always->getParamNames().empty());
        CPPUNIT_ASSERT(!always->setParam("", ""));
        bool ok;
        CPPUNIT_ASSERT_EQUAL(std::string(""), always->getParam("", &ok));
        CPPUNIT_ASSERT(!ok);

        // The Always Dump policy isn't active yet: the buffer is still loaded
        core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();
        bo->allocate(1);
        core::memory::BufferInfo info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(info.loaded);

        // The Always Dump policy is active: the buffer is immediately dumped
        manager->setDumpPolicy(always);
        info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(!info.loaded);

        // Lock request to modify the buffer: temporarily restore then redump the buffer
        *static_cast<char*>(bo->lock().getBuffer()) = '!';
        info                                        = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(!info.loaded);

        // Reallocation: the buffer should be dumped after the reallocation
        bo->reallocate(2);
        info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        //CPPUNIT_ASSERT(!info.loaded); // TODO: Make it work

        bo->destroy();
    }

    // Barrier Dump
    {
        core::memory::IPolicy::sptr barrier                 = core::memory::policy::BarrierDump::New();
        const core::memory::IPolicy::ParamNamesType& params = barrier->getParamNames();
        CPPUNIT_ASSERT(params.size() == 1 && params[0] == "barrier");
        CPPUNIT_ASSERT(!barrier->setParam("banner", "nope"));
        CPPUNIT_ASSERT(!barrier->setParam("barrier", "-1B"));
        CPPUNIT_ASSERT(barrier->setParam("barrier", "1B"));
        manager->setDumpPolicy(barrier);

        // Allocation of a buffer: The managed memory size is still acceptable
        core::memory::BufferObject::sptr bo1 = core::memory::BufferObject::New();
        bo1->allocate(1);
        core::memory::BufferManager::BufferInfoMapType infos = manager->getBufferInfos().get();
        CPPUNIT_ASSERT(infos[bo1->getBufferPointer()].loaded);

        // Lock request to modify a buffer: Shouldn't change its loading status
        *static_cast<char*>(bo1->lock().getBuffer()) = '!';
        infos                                        = manager->getBufferInfos().get();
        CPPUNIT_ASSERT(infos[bo1->getBufferPointer()].loaded);

        // Allocation of a second buffer: The managed memory size exceed the barrier, one of the
        // buffer is evicted from managed memory
        core::memory::BufferObject::sptr bo2 = core::memory::BufferObject::New();
        bo2->allocate(1);
        infos = manager->getBufferInfos().get();
        CPPUNIT_ASSERT(infos[bo1->getBufferPointer()].loaded ^ infos[bo2->getBufferPointer()].loaded);

        // Reallocation of both buffer: No buffer fits the barrier, both buffers should be evicted
        // from managed memory (TODO: Make it work)
        /*bo1->reallocate(2);
           bo2->reallocate(2);
           infos = manager->getBufferInfos().get();
           CPPUNIT_ASSERT(!infos[bo1->getBufferPointer()].loaded && !infos[bo2->getBufferPointer()].loaded);*/

        bo1->destroy();
        bo2->destroy();
    }

    // Never Dump
    {
        core::memory::IPolicy::sptr never = core::memory::policy::NeverDump::New();
        CPPUNIT_ASSERT(never->getParamNames().empty());
        CPPUNIT_ASSERT(!never->setParam("", ""));
        bool ok;
        CPPUNIT_ASSERT_EQUAL(std::string(""), never->getParam("", &ok));
        CPPUNIT_ASSERT(!ok);

        // The buffer isn't dumped on allocation
        core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();
        bo->allocate(1);
        core::memory::BufferInfo info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(info.loaded);

        // The buffer isn't dumped when activating the new policy
        manager->setDumpPolicy(never);
        info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(info.loaded);

        // Lock request to modify the buffer: the buffer still isn't dumped
        *static_cast<char*>(bo->lock().getBuffer()) = '!';
        info                                        = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(info.loaded);

        // Reallocation: the buffer still isn't dumped
        bo->reallocate(2);
        info = manager->getBufferInfos().get().at(bo->getBufferPointer());
        CPPUNIT_ASSERT(info.loaded);

        bo->destroy();
    }

    // Valve Dump
    {
        core::memory::IPolicy::sptr valve = core::memory::policy::ValveDump::New();
        CPPUNIT_ASSERT_EQUAL(size_t(2), valve->getParamNames().size());
        CPPUNIT_ASSERT(!valve->setParam("min_free_memes", "nope"));
        CPPUNIT_ASSERT(!valve->setParam("hysteric_offset", "nope"));
        CPPUNIT_ASSERT(!valve->setParam("min_free_mem", "-1B"));
        CPPUNIT_ASSERT(!valve->setParam("hysteresis_offset", "-1B"));
        CPPUNIT_ASSERT(valve->setParam("min_free_mem", "1B"));
        CPPUNIT_ASSERT(valve->setParam("hysteresis_offset", "1B"));

        // TODO: Currently it is hard to test Valve Dump as it depends on the memory of the host
        // system. A Mock implementation of the MemoryMonitorTools should be created.
    }
}

} // namespace ut

} // namespace sight::core::memory
