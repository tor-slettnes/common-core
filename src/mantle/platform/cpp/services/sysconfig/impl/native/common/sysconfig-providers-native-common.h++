// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-common.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::sysconfig::native
{
    void register_common_providers();
    void unregister_common_providers();
}  // namespace cc::platform::sysconfig::native
