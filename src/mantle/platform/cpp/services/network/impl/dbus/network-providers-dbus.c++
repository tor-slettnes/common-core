// -*- c++ -*-
//==============================================================================
/// @file network-providers-dbus.c++
/// @brief Network service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-providers-dbus.h++"
#include "network-dbus.h++"

#include "giomm/init.h"

namespace platform::network::dbus
{
    void register_providers()
    {
        Gio::init();

        network.registerProvider<DBusProvider>();
    }

    void unregister_providers()
    {
        network.unregisterProvider<DBusProvider>();
    }
}  // namespace platform::network::dbus
