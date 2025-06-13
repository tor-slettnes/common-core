/// -*- c++ -*-
//==============================================================================
/// @file init.c++
/// @brief Application startup/shutdown
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "init.h++"
#include "logging/logging.h++"

namespace core::platform
{
    //==========================================================================
    // Task

    Task::Task(const std::string &handle, const Function &functor)
        : Function(functor),
          handle(handle)
    {
    }

    //==========================================================================
    // InitTask

    InitTask::InitTask(const std::string &handle, const Function &functor)
        : Task(handle, functor)
    {
        init_tasks.insert(this);
    }

    InitTask::~InitTask()
    {
        init_tasks.erase(this);
    }

    //==========================================================================
    // ShutdownTask

    ShutdownTask::ShutdownTask(const std::string &handle, const Function &functor)
        : Task(handle, functor)
    {
        signal_shutdown.connect(handle, *this);
    }

    ShutdownTask::~ShutdownTask()
    {
        signal_shutdown.disconnect(handle);
    }

    //==========================================================================
    // ExitTask

    ExitTask::ExitTask(const std::string &handle, const Function &functor)
        : Task(handle, functor)
    {
        exit_tasks.insert(this);
    }

    ExitTask::~ExitTask()
    {
        exit_tasks.erase(this);
    }

    //==========================================================================
    // TaskSet

    void TaskSet::execute() const
    {
        for (const Task *task : *this)
        {
           log_trace("Executing task: ", task->handle);
            (*task)();
        }
    }

    //==========================================================================
    // Global symbols

    // TaskSet init_tasks;
    // TaskSet exit_tasks;

    core::signal::AsyncVoidSignal signal_startup("signal_startup");
    core::signal::AsyncVoidSignal signal_shutdown("signal_shutdown");
}  // namespace core::platform
