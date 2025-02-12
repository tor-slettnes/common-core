// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-posix.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-posix.h++"
#include "sysconfig-providers-native-common.h++"
#include "sysconfig-posix-timezone.h++"
#include "sysconfig-posix-time.h++"

namespace platform::sysconfig::native
{
    void register_posix_providers()
    {
        timezone.registerProvider<PosixTimeZoneProvider>();
        time.registerProvider<PosixTimeConfigProvider>();
    }

    void unregister_posix_providers()
    {
        timezone.unregisterProvider<PosixTimeConfigProvider>();
        time.unregisterProvider<PosixTimeZoneProvider>();
    }
}  // namespace platform::sysconfig::native
