// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-linux.h++
/// @brief SysConfig service - Linux native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::sysconfig::native
{
    void register_linux_providers();
    void unregister_linux_providers();
}  // namespace cc::platform::sysconfig::native
