// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-native.c++
/// @brief Upgrade service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-providers-native.h++"
#include "upgrade-native-provider.h++"

namespace platform::upgrade::native
{
    void register_providers()
    {
        upgrade.registerProvider<NativeProvider>();
    }

    void unregister_providers()
    {
        upgrade.unregisterProvider<NativeProvider>();
    }
}  // namespace platform::upgrade::native
