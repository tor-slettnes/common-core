/// -*- c++ -*-
//==============================================================================
/// @file dbus-proxywrapper-properties.c++
/// @brief Abstract base for DBus Interface handlers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dbus-proxywrapper.h++"

namespace core::dbus
{
    class PropertiesProxy : public ProxyWrapper
    {
    public:
        PropertiesProxy(
            ProxyContainer* container,
            const dbus::ConnectionPtr& connection,
            const dbus::ServiceName& servicename,
            const dbus::ObjectPath& objectpath,
            const dbus::InterfaceName& interfacename);

        void set_property(const std::string& propertyname,
                          const Glib::VariantBase& value);

    protected:
        dbus::InterfaceName real_interface;
    };
}  // namespace core::dbus
