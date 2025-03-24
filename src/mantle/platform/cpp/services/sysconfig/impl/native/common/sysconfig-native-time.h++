// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-time.h++
/// @brief SysConfig native implementation - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-time.h++"

namespace sysconfig::native
{
    class TimeConfigProvider : public TimeConfigInterface
    {
        using This = TimeConfigProvider;
        using Super = TimeConfigInterface;

    public:
        TimeConfigProvider(const std::string &name = "TimeConfigProvider");

        void initialize() override;
        void deinitialize() override;

    protected:
        // Current timestamp
        core::dt::TimePoint get_current_time() const override;
        void set_current_time(const core::dt::TimePoint &tp) override;

        // Get or set time configuration
        void set_time_config(const TimeConfig &config) override;
        TimeConfig get_time_config() const override;

    private:
        void emit_time_config() const;
    };
}  // namespace sysconfig::native
