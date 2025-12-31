// -*- c++ -*-
//==============================================================================
/// @file sysconfig-host.c++
/// @brief SysConfig service - Host Configuration API
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-host.h++"

namespace sysconfig
{
    core::types::TaggedValueList &operator<<(
        core::types::TaggedValueList &tvlist,
        const HostInfo &hi)
    {
        return tvlist.extend({
            {"hostname", hi.hostname},
            {"os_name", hi.os_name},
            {"os_release_flavor_short", hi.os_release_flavor_short},
            {"os_release_flavor_long", hi.os_release_flavor_long},
            {"os_release_version_id", hi.os_release_version_id},
            {"os_release_version_name", hi.os_release_version_name},
            {"machine_arch", hi.machine_arch},
            {"machine_cpu", hi.machine_cpu},
            {"machine_name", hi.machine_name},
            {"machine_description", hi.machine_description},
            {"machine_serial", hi.machine_serial},
            {"machine_uid", hi.machine_uid},
        });
    }

    std::ostream &operator<<(
        std::ostream &stream,
        const HostInfo &hi)
    {
        return stream << core::types::TaggedValueList::create_from(hi);
    }

    //==========================================================================
    // Provider instance

    core::platform::ProviderProxy<HostConfigInterface> host("host");

    //==========================================================================
    // Signals

    core::signal::DataSignal<HostInfo> signal_hostinfo("hostinfo", true);
}  // namespace sysconfig
