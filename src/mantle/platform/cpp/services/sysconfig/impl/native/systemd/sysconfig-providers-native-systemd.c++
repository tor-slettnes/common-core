// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-systemd.c++
/// @brief SysConfig service - SystemD implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-systemd.h++"
#include "sysconfig-systemd-time.h++"

namespace sysconfig::native
{
    void register_systemd_providers()
    {
        time.registerProvider<SystemdTimeConfigProvider>();
    }

    void unregister_systemd_providers()
    {
        time.unregisterProvider<SystemdTimeConfigProvider>();
    }
}  // namespace sysconfig::native
