/// -*- c++ -*-
//==============================================================================
/// @file nm-device-wired.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager wired device

    class WiredDevice : public DataWrapper<WiredDeviceData>
    {
        using Class = WiredDevice;
        using Super = DataWrapper<WiredDeviceData>;

        friend class Device;

    public:
        WiredDevice(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionRef& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);
        void emit_change(core::signal::MappingAction action) override;

        static std::shared_ptr<WiredDevice> first();

    private:
        void on_property_hwaddress(
            const Glib::VariantBase& change);

        void on_property_active_connection(
            const Glib::VariantBase& change);
    };

}  // namespace platform::netconfig::dbus
