/// -*- c++ -*-
//==============================================================================
/// @file nm-wrappers.c++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "nm-wrappers.h++"

namespace platform::network::dbus
{
    //==========================================================================
    // NetworkManager proxy container

    core::dbus::ProxyContainer container(Gio::DBus::BUS_TYPE_SYSTEM, NM_DBUS_SERVICE);
}  // namespace platform::network::dbus
