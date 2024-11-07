/// -*- c++ -*-
//==============================================================================
/// @file dbus-types.h++
/// @brief Type definitions used to wrap Glib's DBus interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "glib-variant.h++"
#include "logging/logging.h++"

#include <glibmm/refptr.h>
#include <giomm/dbusconnection.h>
#include <giomm/dbusproxy.h>
#include <giomm/dbusobjectproxy.h>

#define SLOT(method)  sigc::mem_fun(this, &method)
#define DATASLOT(ptr) [&](const Glib::VariantBase& v) { \
    core::glib::variant_cast(v, ptr);                   \
}

namespace core::dbus
{
    define_log_scope("dbus");
    using ConnectionPtr = Glib::RefPtr<Gio::DBus::Connection>;
    using ServiceName = std::string;
    using ObjectPath = Glib::DBusObjectPathString;  // <-- Non-hashable

    //using ObjectPath = std::string;
    using ObjectPaths = std::vector<ObjectPath>;
    using ObjectPathVariant = Glib::Variant<Glib::DBusObjectPathString>;

    using InterfaceName = std::string;
    using SignalName = std::string;
    using PropertyName = std::string;

    class ProxyContainer;
    class ProxyWrapper;
    class PropertiesProxyWrapper;
}  // namespace core::dbus
