// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-time.c++
/// @brief SysConfig native implementation - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-native-time.h++"
#include "platform/time.h++"
#include "platform/symbols.h++"
#include "chrono/scheduler.h++"
#include "chrono/date-time.h++"

namespace platform::sysconfig::native
{
    TimeConfigProvider::TimeConfigProvider()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    void TimeConfigProvider::initialize()
    {
        Super::initialize();
        logf_info("Scheduling time updates each second");
        core::scheduler.add(
            this->name(),                        // handle
            [](const core::dt::TimePoint &tp) {  // |
                signal_time.emit(tp);            // |> Invocation
            },                                   // |
            1s,                                  // Interval
            core::Scheduler::ALIGN_UTC,          // alignment
            core::status::Level::TRACE);         // loglevel

        this->emit_time_config();
    }

    void TimeConfigProvider::deinitialize()
    {
        logf_info("Unscheduling time updates");
        core::scheduler.remove(this->name());
        Super::deinitialize();
    }

    void TimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        core::platform::time->set_time(tp);
    }

    core::dt::TimePoint TimeConfigProvider::get_current_time() const
    {
        return core::dt::Clock::now();
    }

    void TimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        core::platform::time->set_ntp(config.synchronization == TSYNC_NTP);
        if (!config.servers.empty())
        {
            core::platform::time->set_ntp_servers(config.servers);
        }
    }

    TimeConfig TimeConfigProvider::get_time_config() const
    {
        return {
            .synchronization = (core::platform::time->get_ntp() ? TSYNC_NTP : TSYNC_NONE),
            .servers = core::platform::time->get_ntp_servers(),
        };
    }

    void TimeConfigProvider::emit_time_config() const
    {
        signal_timeconfig.emit(this->get_time_config());
    }

}  // namespace platform::sysconfig::native
