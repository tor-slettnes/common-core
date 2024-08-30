// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-common.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::sysconfig::native
{
    void register_common_providers();
    void unregister_common_providers();
}  // namespace platform::sysconfig::native
