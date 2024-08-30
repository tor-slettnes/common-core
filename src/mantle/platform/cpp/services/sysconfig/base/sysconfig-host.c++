// -*- c++ -*-
//==============================================================================
/// @file sysconfig-host.c++
/// @brief SysConfig service - Host Configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-host.h++"
#include "types/partslist.h++"

namespace platform::sysconfig
{
    std::ostream &operator<<(std::ostream &stream, const HostInfo &hi)
    {
        core::types::PartsList parts;
        parts.add_string("hostname", hi.hostname);
        parts.add_string("os_name", hi.os_name);
        parts.add_string("os_release_flavor_short", hi.os_release_flavor_short);
        parts.add_string("os_release_flavor_long", hi.os_release_flavor_long);
        parts.add_string("os_release_version_id", hi.os_release_version_id);
        parts.add_string("os_release_version_name", hi.os_release_version_name);
        parts.add_string("machine_arch", hi.machine_arch);
        parts.add_string("machine_cpu", hi.machine_cpu);
        parts.add_string("machine_name", hi.machine_name);
        parts.add_string("machine_description", hi.machine_description);
        parts.add_string("machine_serial", hi.machine_serial);
        parts.add_string("machine_uid", hi.machine_uid);
        stream << parts;
        return stream;
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<HostConfigInterface> host("host");

    //==========================================================================
    // Signals

    core::signal::DataSignal<HostInfo> signal_hostinfo("hostinfo", true);
}  // namespace platform::sysconfig
