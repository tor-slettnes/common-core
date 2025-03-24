// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native-linux.h++
/// @brief SysConfig service - Linux native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace sysconfig::native
{
    void register_linux_providers();
    void unregister_linux_providers();
}  // namespace sysconfig::native
