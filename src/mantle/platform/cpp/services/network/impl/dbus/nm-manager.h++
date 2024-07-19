// -*- c++ -*-
//==============================================================================
/// @file nm-manager.h++
/// @brief Network service - Network Manager 'Manager' object wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dbus-proxywrapper.h++"
#include "settings/settingsstore.h++"
#include "nm-wrappers.h++"
#include "nm-settings.h++"
#include "nm-settings-connection.h++"
#include "nm-connection-active.h++"
#include "nm-accesspoint.h++"
#include "nm-device.h++"

#include <nm-dbus-interface.h>

namespace platform::network::dbus
{
    class Manager : public DataWrapper<GlobalData>
    {
        using Class = Manager;
        using Super = core::dbus::ProxyWrapper;

    public:
        Manager(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionRef& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath = NM_DBUS_PATH);

    protected:
        void initialize() override;
        void emit_change(core::signal::MappingAction action) override;

    public:
        void set_wireless_enabled(bool enabled);
        void set_wireless_allowed(bool allowed);
        void select_wireless_band(WirelessBandSelection band);
        void request_scan() const;

        void add_and_activate_connection(ConnectionData data);

        void add_and_activate_connection(
            const ConnectionData& data,
            const Glib::DBusObjectPathString& device_path,
            const Glib::DBusObjectPathString& specific_path = "/");

        void activate_connection(const Key& key);
        void deactivate_connection(const Key& key);
        void connect_ap(const Key& bssid, const ConnectionData& data = {});
        void connect_ap(const SSID& ssid, const ConnectionData& data = {});

    private:
        void connect_ap(std::shared_ptr<WirelessDevice> dev,
                        std::shared_ptr<AccessPoint> ap,
                        const ConnectionData& data);

        void on_signal_state_changed(const Glib::VariantContainerBase& parameters);
        void on_property_devices(const Glib::VariantBase& change);
        void on_property_wireless_enabled(const Glib::VariantBase& change);
        void on_property_active_connections(const Glib::VariantBase& change);

    private:
        std::shared_ptr<core::dbus::PropertiesProxy> properties_proxy;
        core::SettingsStore settings;
    };
}  // namespace platform::network::dbus
