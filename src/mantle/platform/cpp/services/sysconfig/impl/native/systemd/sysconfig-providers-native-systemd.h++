// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-systemd.h++
/// @brief SysConfig service - SystemD implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::sysconfig::native
{
    void register_systemd_providers();
    void unregister_systemd_providers();
}  // namespace cc::platform::sysconfig::native
