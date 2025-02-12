// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-linux.h++
/// @brief SysConfig service - SystemD implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::sysconfig::native
{
    void register_systemd_providers();
    void unregister_systemd_providers();
}  // namespace platform::sysconfig::native
