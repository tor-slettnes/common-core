// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-dbus.h++
/// @brief NetConfig service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::netconfig::dbus
{
    void register_providers();
    void unregister_providers();
}  // namespace platform::netconfig::dbus
