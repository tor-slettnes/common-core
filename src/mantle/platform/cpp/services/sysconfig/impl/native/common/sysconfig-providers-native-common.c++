// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-common.h++"
#include "sysconfig-native-time.h++"
#include "sysconfig-native-host.h++"
#include "sysconfig-native-process.h++"

namespace platform::sysconfig::native
{
    void register_common_providers()
    {
        time.registerProvider<TimeConfigProvider>();
        hostconfig.registerProvider<HostConfigProvider>();
        process.registerProvider<ProcessProvider>();
    }

    void unregister_common_providers()
    {
        process.unregisterProvider<ProcessProvider>();
        hostconfig.unregisterProvider<HostConfigProvider>();
        time.unregisterProvider<TimeConfigProvider>();
    }
}  // namespace platform::sysconfig
