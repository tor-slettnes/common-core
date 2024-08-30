// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-posix.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-posix.h++"
#include "sysconfig-providers-native-common.h++"
#include "sysconfig-posix-timezone.h++"

namespace platform::sysconfig::native
{
    void register_posix_providers()
    {
        timezone.registerProvider<PosixTimeZoneProvider>();
    }

    void unregister_posix_providers()
    {
        timezone.unregisterProvider<PosixTimeZoneProvider>();
    }
}  // namespace platform::sysconfig::native
