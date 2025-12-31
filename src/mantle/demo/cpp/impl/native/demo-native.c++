// -*- c++ -*-
//==============================================================================
/// @file demo-native.c++
/// @brief Example app - Local/native implementation
/// @author Tor Slettnes
//==============================================================================

#include "demo-native.h++"
#include "demo-signals.h++"
#include "chrono/scheduler.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"

namespace demo
{
    constexpr const char *SIGNAL_HANDLE = "demo-notify";
    constexpr const char *TIMER_TASK_HANDLE = "demo-timer";

    NativeImpl::NativeImpl(const std::string &identity)
        : Super(identity, "Native")
    {
    }

    void NativeImpl::say_hello(const Greeting &greeting)
    {
        log_notice("Received and redistributing greeting: ", greeting);
        /// Emit `signal_greeting` to registered slots/callbacks.
        if (core::str::tolower(greeting.identity) == "dave")
        {
            throw core::exception::InvalidArgument(
                "I'm sorry Dave, I cannot let you do that.");
        }
        else
        {
            demo::signal_greeting.emit(greeting.identity, greeting);
        }
    }

    TimeData NativeImpl::get_current_time()
    {
        return {core::dt::Clock::now()};
    }

    void NativeImpl::start_ticking()
    {
        // Schedule a task to emit a new TimeData update every second
        log_notice("Starting periodic time updates");
        core::scheduler.add_if_missing(
            TIMER_TASK_HANDLE,
            [](const core::dt::TimePoint &tp) {
                demo::signal_time.emit(TimeData(tp));
            },
            std::chrono::seconds(1),
            core::Scheduler::ALIGN_UTC);
    }

    void NativeImpl::stop_ticking()
    {
        // Cancel any existing TimeData update task
        log_notice("Stopping periodic time updates");
        core::scheduler.remove(TIMER_TASK_HANDLE);
    }
}  // namespace demo
