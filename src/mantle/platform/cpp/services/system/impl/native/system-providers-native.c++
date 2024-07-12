// -*- c++ -*-
//==============================================================================
/// @file system-providers-native.c++
/// @brief System service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-providers-native.h++"
#include "system-native-time.h++"
#include "system-native-host.h++"

namespace platform::system::native
{
    void register_providers()
    {
        time.registerProvider<Time>();
        hostconfig.registerProvider<HostConfig>();
    }

    void unregister_providers()
    {
        hostconfig.unregisterProvider<HostConfig>();
        time.unregisterProvider<Time>();
    }
}  // namespace platform::system
