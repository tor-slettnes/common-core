// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-host.c++
/// @brief SysConfig native implementation - Host Configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-native-host.h++"
#include "platform/host.h++"
#include "platform/symbols.h++"

namespace sysconfig::native
{
    HostConfigProvider::HostConfigProvider()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    void HostConfigProvider::initialize()
    {
        Super::initialize();
        this->emit();
    }

    //==========================================================================
    // HostConfigProvider provider

    HostInfo HostConfigProvider::get_host_info() const
    {
        return {
            .hostname = core::platform::host->get_host_name(),
            .os_name = core::platform::host->get_system_name(),
            .os_version = core::platform::host->get_system_version(),
            .os_release_flavor_short = core::platform::host->get_system_release_flavor_short(),
            .os_release_flavor_long = core::platform::host->get_system_release_flavor_long(),
            .os_release_version_id = core::platform::host->get_system_release_version_id(),
            .os_release_version_name = core::platform::host->get_system_release_version_name(),
        };
    }

    void HostConfigProvider::set_host_name(const std::string &hostname)
    {
        core::platform::host->set_host_name(hostname);
        this->emit();
    }

    void HostConfigProvider::reboot()
    {
        core::platform::host->reboot();
    }

    void HostConfigProvider::emit() const
    {
        signal_hostinfo.emit(this->get_host_info());
    }

}  // namespace sysconfig::native
