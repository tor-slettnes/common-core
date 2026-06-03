/// -*- c++ -*-
//==============================================================================
/// @file nm-wrappers.c++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes
//==============================================================================

#include "nm-wrappers.h++"

namespace cc::platform::netconfig::dbus
{
    //==========================================================================
    // NetworkManager proxy container

    cc::dbus::ProxyContainer container(Gio::DBus::BUS_TYPE_SYSTEM, NM_DBUS_SERVICE);
}  // namespace cc::platform::netconfig::dbus
