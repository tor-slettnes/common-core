// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-time.h++
/// @brief SysConfig service - Time Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-time.h++"

namespace platform::sysconfig::native
{
    class TimeConfigProvider : public TimeConfigInterface
    {
        using This = TimeConfigProvider;
        using Super = TimeConfigInterface;

    public:
        TimeConfigProvider();

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
}  // namespace platform::sysconfig::native
