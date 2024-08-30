// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-posix.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::sysconfig::native
{
    void register_posix_providers();
    void unregister_posix_providers();
}  // namespace platform::sysconfig::native
