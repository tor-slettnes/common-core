/// -*- c++ -*-
//==============================================================================
/// @file init.h++
/// @brief Application startup/shutdown
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "thread/signaltemplate.h++"

#include <unordered_set>

namespace core::platform
{
    //==========================================================================
    // InitTask

    class Task : public std::function<void()>
    {
    protected:
        using Function = std::function<void()>;

        Task(const std::string &handle,
             const Function &functor);

    public:
        std::string handle;
    };

    // using Task = std::function<void()>;

    //==========================================================================
    // InitTask

    class InitTask : public Task
    {
    public:
        InitTask(const std::string &handle,
                 const Function &functor);
        ~InitTask();
    };

    //==========================================================================
    // ExitTask

    class ExitTask : public Task
    {
    public:
        ExitTask(const std::string &handle,
                 const Function &functor);
        ~ExitTask();
    };

    //==========================================================================
    // TaskSet

    class TaskSet : public std::unordered_set<Task *>
    {
    public:
        void execute() const;
    };

    //==========================================================================
    // Global symbols

    inline TaskSet init_tasks;
    inline TaskSet exit_tasks;

    extern core::signal::VoidSignal signal_startup;
    extern core::signal::VoidSignal signal_shutdown;
}  // namespace core::platform
