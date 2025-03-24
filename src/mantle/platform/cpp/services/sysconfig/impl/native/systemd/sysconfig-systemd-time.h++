// -*- c++ -*-
//==============================================================================
/// @file sysconfig-systemd-time.h++
/// @brief SysConfig implementation via SystemD - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-posix-time.h++"

namespace sysconfig::native
{
    class SystemdTimeConfigProvider : public PosixTimeConfigProvider
    {
        using This = SystemdTimeConfigProvider;
        using Super = PosixTimeConfigProvider;

    public:
        SystemdTimeConfigProvider();

        bool is_pertinent() const override;

    protected:
        // Current timestamp
        void set_current_time(const core::dt::TimePoint &tp) override;
        void set_ntp(bool ntp);
        bool get_ntp() const;

        std::vector<std::string> get_ntp_servers() const;
        core::types::ValueMap<std::string, std::string> read_settings() const;

    };
}  // namespace sysconfig::systemd
