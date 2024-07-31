/// -*- c++ -*-
//==============================================================================
/// @file time.c++
/// @brief Date/Time functions - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "time.h++"

namespace core::platform
{
    void TimeProvider::set_ntp(bool ntp)
    {
        throw std::invalid_argument("set_ntp() is not implemented on this platform");
    }

    bool TimeProvider::get_ntp() const
    {
        throw std::invalid_argument("get_ntp() is not implemented on this platform");
    }

    void TimeProvider::set_ntp_servers(const std::vector<std::string> &servers)
    {
        throw std::invalid_argument("set_ntp_servers() is not implemented on this platform");
    }

    std::vector<std::string> TimeProvider::get_ntp_servers() const
    {
        throw std::invalid_argument("get_ntp_servers() is not implemented on this platform");
    }

    ProviderProxy<TimeProvider> time("time");
}  // namespace core::platform
