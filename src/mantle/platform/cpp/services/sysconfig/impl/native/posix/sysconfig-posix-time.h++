// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-time.h++
/// @brief SysConfig Posix implementation - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-native-time.h++"

namespace platform::sysconfig::native
{
    class PosixTimeConfigProvider : public native::TimeConfigProvider
    {
        using This = PosixTimeConfigProvider;
        using Super = TimeConfigProvider;

    public:
        using Super::Super;

    protected:
        TimeConfig get_time_config() const override;
        void set_time_config(const TimeConfig &config) override;

        virtual void set_ntp(bool ntp);
        virtual bool get_ntp() const;

        virtual void set_ntp_servers(const std::vector<std::string> &servers);
        virtual std::vector<std::string> get_ntp_servers() const;
    };
}  // namespace platform::sysconfig::posix
