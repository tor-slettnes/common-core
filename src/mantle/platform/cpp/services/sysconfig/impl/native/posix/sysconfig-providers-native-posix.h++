// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-posix.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::sysconfig::native
{
    void register_posix_providers();
    void unregister_posix_providers();
}  // namespace cc::platform::sysconfig::native
