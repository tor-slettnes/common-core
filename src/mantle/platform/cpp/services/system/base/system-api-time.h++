// -*- c++ -*-
//==============================================================================
/// @file system-api-time.h++
/// @brief System service - Abstract C++ interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-types.h++"
#include "platform/provider.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace platform::system
{
    class TimeProvider : public core::platform::Provider
    {
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

    extern core::platform::ProviderProxy<TimeProvider> time;

    extern core::signal::DataSignal<core::dt::TimePoint> signal_time;
    extern core::signal::DataSignal<TimeConfig> signal_timeconfig;

}  // namespace platform::system
