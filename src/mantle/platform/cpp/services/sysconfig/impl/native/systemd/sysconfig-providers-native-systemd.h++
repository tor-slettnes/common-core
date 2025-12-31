// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-linux.h++
/// @brief SysConfig service - SystemD implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace sysconfig::native
{
    void register_systemd_providers();
    void unregister_systemd_providers();
}  // namespace sysconfig::native
