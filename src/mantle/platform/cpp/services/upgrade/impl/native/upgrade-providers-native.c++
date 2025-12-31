// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-native.c++
/// @brief Upgrade service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-providers-native.h++"
#include "upgrade-native-provider.h++"

namespace upgrade::native
{
    void register_providers()
    {
        upgrade.registerProvider<NativeProvider>();
    }

    void unregister_providers()
    {
        upgrade.unregisterProvider<NativeProvider>();
    }
}  // namespace upgrade::native
