// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::sysconfig::native
{
    void register_providers();
    void unregister_providers();
}  // namespace cc::platform::sysconfig::native
