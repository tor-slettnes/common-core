// -*- c++ -*-
//==============================================================================
/// @file system-native-time.h++
/// @brief System service - Time Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-api-time.h++"

namespace platform::system::native
{
    class Time : public TimeProvider
    {
        using Super = TimeProvider;

    public:
        Time();

        void initialize() override;
        void deinitialize() override;

    protected:
        // Current timestamp
        void set_current_time(const core::dt::TimePoint &tp) override;
        core::dt::TimePoint get_current_time() const override;

        // Get or set time configuration
        void set_time_config(const TimeConfig &config) override;
        TimeConfig get_time_config() const override;
    };
}  // namespace platform::system::native
