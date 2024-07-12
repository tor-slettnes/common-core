// -*- c++ -*-
//==============================================================================
/// @file system-native-time.h++
/// @brief System service - Time Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-native-time.h++"
#include "platform/time.h++"
#include "chrono/scheduler.h++"
#include "chrono/date-time.h++"

namespace platform::system::native
{
    Time::Time()
        : Super("native::Time")
    {
    }

    void Time::initialize()
    {
        Super::initialize();
        logf_info("Scheduling time updates each second");
        core::scheduler.add(
            this->name(),                  // handle
            [](const core::dt::TimePoint &tp) {  // |
                signal_time.emit(tp);      // > Invocation
            },                             // |
            1s,                            // Interval
            core::Scheduler::ALIGN_UTC,    // alignment
            core::status::Level::TRACE);   // loglevel
    }

    void Time::deinitialize()
    {
        logf_info("Unscheduling time updates");
        core::scheduler.remove(this->name());
        Super::deinitialize();
    }

    void Time::set_current_time(const core::dt::TimePoint &tp)
    {
        core::platform::time->set_time(tp);
    }

    core::dt::TimePoint Time::get_current_time() const
    {
        return core::dt::Clock::now();
    }

    void Time::set_time_config(const TimeConfig &config)
    {
    }

    TimeConfig Time::get_time_config() const
    {
        return {};
    }

}  // namespace platform::system::native
