/// -*- c++ -*-
//==============================================================================
/// @file nm-device-wireless.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"
#include "nm-accesspoint.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager WiFi device

    class WirelessDevice : public DataWrapper<WirelessDeviceData>
    {
        using Class = WirelessDevice;
        using Super = DataWrapper<WirelessDeviceData>;
        friend class Device;

    public:
        WirelessDevice(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionRef& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

        static std::shared_ptr<WirelessDevice> get_by_ap(
            const std::shared_ptr<AccessPoint> ap,
            bool required);

        static std::shared_ptr<WirelessDevice> first();
        void request_scan() const;

    protected:
        void initialize() override;
        void emit_change(core::signal::MappingAction action) override;

    private:
        void on_signal_accesspoint_added(
            const Glib::VariantContainerBase& parameters);

        void on_signal_accesspoint_removed(
            const Glib::VariantContainerBase& parameters);

        void on_property_active_accesspoint(
            const Glib::VariantBase& change);

        void on_property_lastscan(
            const Glib::VariantBase& change);

        void on_property_hwaddress(
            const Glib::VariantBase& change);

        void on_property_active_connection(
            const Glib::VariantBase& change);

        bool update_active_accesspoint(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

    private:
        std::set<core::dbus::ObjectPath> accesspoints;
    };

}  // namespace platform::netconfig::dbus
