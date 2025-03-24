// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-dbus.c++
/// @brief NetConfig service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-providers-dbus.h++"
#include "netconfig-dbus.h++"

#include "giomm/init.h"

namespace netconfig::dbus
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
}  // namespace netconfig::dbus
