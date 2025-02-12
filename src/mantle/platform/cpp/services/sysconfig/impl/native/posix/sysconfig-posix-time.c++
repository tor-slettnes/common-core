// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-time.c++
/// @brief SysConfig Posix implementation - Time Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-posix-time.h++"
#include "status/exceptions.h++"

namespace platform::sysconfig::native
{
    TimeConfig PosixTimeConfigProvider::get_time_config() const
    {
        return {
            .synchronization = (this->get_ntp() ? TSYNC_NTP : TSYNC_NONE),
            .servers = this->get_ntp_servers(),
        };
    }

    void PosixTimeConfigProvider::set_time_config(const TimeConfig &config)
    {
        this->set_ntp(config.synchronization == TSYNC_NTP);
        if (!config.servers.empty())
        {
            this->set_ntp_servers(config.servers);
        }
    }

    void PosixTimeConfigProvider::set_ntp(bool ntp)
    {
        throw core::exception::UnsupportedError(
            "set_ntp() is not implemented on this platform");
    }

    bool PosixTimeConfigProvider::get_ntp() const
    {
        throw core::exception::UnsupportedError(
            "get_ntp() is not implemented on this platform");
    }

    void PosixTimeConfigProvider::set_ntp_servers(const std::vector<std::string> &servers)
    {
        throw core::exception::UnsupportedError(
            "set_ntp_servers() is not implemented on this platform");
    }

    std::vector<std::string> PosixTimeConfigProvider::get_ntp_servers() const
    {
        throw core::exception::UnsupportedError(
            "get_ntp_servers() is not implemented on this platform");
    }
}  // namespace platform::sysconfig::native
