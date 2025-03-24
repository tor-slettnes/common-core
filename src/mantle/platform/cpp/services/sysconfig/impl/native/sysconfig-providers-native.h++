// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-native.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace sysconfig::native
{
    void register_providers();
    void unregister_providers();
}  // namespace sysconfig::native
