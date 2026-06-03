// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-dbus.c++
/// @brief NetConfig service - D-BUS implementation
/// @author Tor Slettnes
//==============================================================================

#include "netconfig-providers-dbus.h++"
#include "netconfig-dbus.h++"

#include "giomm/init.h"

namespace cc::platform::netconfig::dbus
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
}  // namespace cc::platform::netconfig::dbus
