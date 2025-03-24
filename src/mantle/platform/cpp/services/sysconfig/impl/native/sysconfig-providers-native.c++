// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-native-systemd.h++"
#include "sysconfig-providers-native-linux.h++"
#include "sysconfig-providers-native-posix.h++"
#include "sysconfig-providers-native-common.h++"

namespace sysconfig::native
{
    void register_providers()
    {
        register_systemd_providers();
        register_linux_providers();
        register_posix_providers();
        register_common_providers();
    }

    void unregister_providers()
    {
        unregister_common_providers();
        unregister_posix_providers();
        unregister_linux_providers();
        unregister_systemd_providers();
    }
}  // namespace sysconfig::native
