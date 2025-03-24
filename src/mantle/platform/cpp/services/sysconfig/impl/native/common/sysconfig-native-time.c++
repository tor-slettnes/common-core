// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-time.c++
/// @brief SysConfig native implementation - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-native-time.h++"
#include "platform/symbols.h++"
#include "chrono/scheduler.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"

namespace sysconfig::native
{
    TimeConfigProvider::TimeConfigProvider(const std::string &name)
        : Super(name)
    {
    }

    void TimeConfigProvider::initialize()
    {
        Super::initialize();
        logf_debug("Scheduling time updates each second");
        core::scheduler.add(
            this->name(),                        // handle
            [](const core::dt::TimePoint &tp) {  // |
                signal_time.emit(tp);            // |> Invocation
            },                                   // |
            1s,                                  // Interval
            core::Scheduler::ALIGN_UTC,          // alignment
            core::status::Level::TRACE);         // loglevel

        try
        {
            this->emit_time_config();
        }
        catch (const core::exception::UnsupportedError &e)
        {
            log_warning(this->name(), ": ", e);
        }
    }

    void TimeConfigProvider::deinitialize()
    {
        logf_debug("Unscheduling time updates");
        core::scheduler.remove(this->name());
        Super::deinitialize();
    }

    core::dt::TimePoint TimeConfigProvider::get_current_time() const
    {
        return core::dt::Clock::now();
    }

    void TimeConfigProvider::set_current_time(const core::dt::TimePoint &tp)
    {
        throw core::exception::UnsupportedError(
            "set_current_time() is not implemented on this platform");
    }

    TimeConfig TimeConfigProvider::get_time_config() const
    {
        throw core::exception::UnsupportedError(
            "get_time_config() is not implemented on this platform");
    }

    void TimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        throw core::exception::UnsupportedError(
            "set_time_config() is not implemented on this platform");
    }

    void TimeConfigProvider::emit_time_config() const
    {
        sysconfig::signal_timeconfig.emit(this->get_time_config());
    }

}  // namespace sysconfig::native
