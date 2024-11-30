// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-common.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-common.h++"
#include "sysconfig-native-time.h++"
#include "sysconfig-native-host.h++"
#include "sysconfig-native-product.h++"
#include "sysconfig-native-process.h++"

namespace platform::sysconfig::native
{
    void register_common_providers()
    {
        time.registerProvider<TimeConfigProvider>();
        host.registerProvider<HostConfigProvider>();
        product.registerProvider<ProductProvider>();
        process.registerProvider<ProcessProvider>();
    }

    void unregister_common_providers()
    {
        process.unregisterProvider<ProcessProvider>();
        product.unregisterProvider<ProductProvider>();
        host.unregisterProvider<HostConfigProvider>();
        time.unregisterProvider<TimeConfigProvider>();
    }
}  // namespace platform::sysconfig::native
