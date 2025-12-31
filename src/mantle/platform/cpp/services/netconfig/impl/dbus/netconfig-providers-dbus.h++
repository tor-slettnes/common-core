// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-dbus.h++
/// @brief NetConfig service - D-BUS implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace netconfig::dbus
{
    void register_providers();
    void unregister_providers();
}  // namespace netconfig::dbus
