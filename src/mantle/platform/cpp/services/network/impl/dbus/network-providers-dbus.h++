// -*- c++ -*-
//==============================================================================
/// @file network-providers-dbus.h++
/// @brief Network service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::network::dbus
{
    void register_providers();
    void unregister_providers();
}  // namespace platform::network::dbus
