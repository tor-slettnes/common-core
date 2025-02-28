/// -*- c++ -*-
//==============================================================================
/// @file host.c++
/// @brief Host related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "host.h++"
#include "buildinfo.h++"

namespace core::platform
{
    std::string HostProvider::get_system_name() const
    {
        return BUILD_OS;
    }

    std::string HostProvider::get_system_version() const
    {
        return BUILD_OS_VERSION;
    }

    std::string HostProvider::get_system_release_flavor_short() const
    {
        return BUILD_OS_RELEASE;
    }

    std::string HostProvider::get_system_release_flavor_long() const
    {
        return this->get_system_name() + " " + this->get_system_release_flavor_short();
    }

    std::string HostProvider::get_system_release_version_id() const
    {
        return {};
    }

    std::string HostProvider::get_system_release_version_name() const
    {
        return {};
    }

    ProviderProxy<HostProvider> host("host");
}  // namespace core::platform
