/// -*- c++ -*-
//==============================================================================
/// @file nm-device.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"
#include "nm-device-wired.h++"
#include "nm-device-wireless.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager device

    class Device : public MappedDataWrapper<DeviceData>
    {
        using Class = Device;
        using Super = MappedDataWrapper<DeviceData>;

        friend class WiredDevice;
        friend class WirelessDevice;

    public:
        Device(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

    protected:
        void initialize() override;
        void emit_change(core::signal::MappingAction action) override;

    private:
        void on_signal_state_changed(const Glib::VariantContainerBase& parameters);

        void on_property_ip4config(const Glib::VariantBase& change);
        void on_property_ip6config(const Glib::VariantBase& change);
        void on_property_active_connection(const Glib::VariantBase& change);

        bool update_ip4config(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

        bool update_ip6config(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

        bool update_active_connection(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

        bool add_specific_data(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

        void update_specific_data(
            const core::dbus::ProxyWrapper* source,
            core::signal::MappingAction action);

    public:
        static std::shared_ptr<Device> get_by_interface(
            const std::string& interface,
            bool required);

        static std::shared_ptr<Device> get_by_active_connection(
            const Key& ackey,
            bool required);

        bool is_available();
        void disconnect();
    };

}  // namespace platform::netconfig::dbus
