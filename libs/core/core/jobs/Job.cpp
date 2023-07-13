/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#define BOOST_SCOPE_EXIT_CONFIG_USE_LAMBDAS

#include "core/jobs/Job.hpp"

#include "core/jobs/exception/Waiting.hpp"

#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

#include <boost/scope_exit.hpp>

#include <memory>

namespace sight::core::jobs
{

//------------------------------------------------------------------------------

Job::sptr Job::New(const std::string& name, Job::Task task, const core::thread::Worker::sptr& worker)
{
    return std::make_shared<Job>(name, task, worker);
}

//------------------------------------------------------------------------------

Job::Job(const std::string& name, Job::Task task, core::thread::Worker::sptr worker) :
    IJob(name),
    m_task(std::move(task)),
    m_worker(std::move(worker))
{
    m_totalWorkUnits = 100;
}

//------------------------------------------------------------------------------

IJob::SharedFuture Job::runImpl()
{
    Task task_copy;
    {
        core::mt::ReadToWriteLock lock(m_mutex);

        if(m_task == nullptr)
        {
            core::mt::UpgradeToWriteLock writeLock(lock);
            this->finishNoLock();
            return std::async([](){});
        }

        task_copy = m_task;
    }

    const auto job_task =
        [task_copy, this]
        {
            task_copy(*this);

            core::mt::WriteLock lock(m_mutex);
            this->finishNoLock();
            m_task = nullptr;
        };

    if(m_worker)
    {
        return m_worker->postTask<void>(job_task);
    }

    job_task();

    return std::async([](){});
}

//------------------------------------------------------------------------------

IJob::SharedFuture Job::cancel()
{
    auto future = this->IJob::cancel();

    core::mt::WriteLock lock(m_mutex);
    if(m_task)
    {
        m_task = nullptr;
    }

    return future;
}

//------------------------------------------------------------------------------

Job::ProgressCallback Job::progressCallback()
{
    return [this](std::uint64_t doneWork)
           {
               this->doneWork(doneWork);
           };
}

//------------------------------------------------------------------------------

core::thread::Worker::sptr Job::getWorker()
{
    // No need to lock : m_worker only writable in constructor
    return m_worker;
}

} //namespace sight::core::jobs
