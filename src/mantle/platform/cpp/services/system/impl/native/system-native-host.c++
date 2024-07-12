// -*- c++ -*-
//==============================================================================
/// @file system-native-host.c++
/// @brief System service - Host Configuration Native Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-native-host.h++"
#include "platform/host.h++"

namespace platform::system::native
{
    HostConfig::HostConfig()
        : Super("HostConfig")
    {
    }

    //==========================================================================
    // HostConfig provider

    HostInfo HostConfig::get_host_info() const
    {
        HostInfo hi;
        hi.set_hostname(core::platform::host->get_host_name());

        hi.set_os_name(core::platform::host->get_system_name());
        hi.set_os_version(core::platform::host->get_system_version());
        hi.set_os_release_flavor_short(core::platform::host->get_system_release_flavor_short());
        hi.set_os_release_flavor_long(core::platform::host->get_system_release_flavor_long());
        hi.set_os_release_version_id(core::platform::host->get_system_release_version_id());
        hi.set_os_release_version_name(core::platform::host->get_system_release_version_name());
        return hi;
    }

    void HostConfig::set_host_name(const std::string &hostname)
    {
        core::platform::host->set_host_name(hostname);
    }

    void HostConfig::reboot()
    {
        core::platform::host->reboot();
    }

}  // namespace platform::system::native
