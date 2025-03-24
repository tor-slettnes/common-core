// -*- c++ -*-
//==============================================================================
/// @file sysconfig-time.h++
/// @brief SysConfig service - Abstract C++ interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace sysconfig
{
    //==========================================================================
    // TimeSync

    enum TimeSync
    {
        TSYNC_NONE,  // No automatic time synchronization
        TSYNC_NTP    // Synchronization via NTP
    };

    std::ostream &operator<<(std::ostream &stream, TimeSync sync);
    std::istream &operator>>(std::istream &stream, TimeSync &sync);

    //==========================================================================
    // TimeConfig

    struct TimeConfig
    {
        TimeSync synchronization = TSYNC_NONE;  // Synchronization scheme
        std::vector<std::string> servers;       // Synchronization services
    };

    std::ostream &operator<<(std::ostream &stream, const TimeConfig &tc);

    //==========================================================================
    // TimeInterface

    class TimeConfigInterface : public core::platform::Provider
    {
        using This = TimeConfigInterface;
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //==========================================================================
        // Time configuration

        // Current timestamp
        virtual void set_current_time(const core::dt::TimePoint &tp) = 0;
        virtual core::dt::TimePoint get_current_time() const = 0;

        // Get or set time configuration
        virtual void set_time_config(const TimeConfig &config) = 0;
        virtual TimeConfig get_time_config() const = 0;
    };

    //==========================================================================
    // Time provider proxy

    extern core::platform::ProviderProxy<TimeConfigInterface> time;

    //==========================================================================
    // Signals

    extern core::signal::DataSignal<core::dt::TimePoint> signal_time;
    extern core::signal::DataSignal<TimeConfig> signal_timeconfig;

}  // namespace sysconfig
