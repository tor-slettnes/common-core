/// -*- c++ -*-
//==============================================================================
/// @file nm-wrappers.c++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes
//==============================================================================

#include "nm-wrappers.h++"

namespace netconfig::dbus
{
    //==========================================================================
    // NetworkManager proxy container

    core::dbus::ProxyContainer container(Gio::DBus::BUS_TYPE_SYSTEM, NM_DBUS_SERVICE);
}  // namespace netconfig::dbus
