/// -*- c++ -*-
//==============================================================================
/// @file nm-settings-connection.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "nm-wrappers.h++"
#include "nm-structs.h++"
#include "types/bytevector.h++"

namespace cc::platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager Connection Settings

    class Connection : public MappedDataWrapper<ConnectionData>
    {
        using Class = Connection;
        using Super = MappedDataWrapper<ConnectionData>;

    public:
        Connection(
            cc::dbus::ProxyContainer* container,
            const cc::dbus::ConnectionPtr& connection,
            const cc::dbus::ServiceName& servicename,
            const cc::dbus::ObjectPath& objectpath);

    public:
        const core::types::ByteVector ssid() const;

        static std::shared_ptr<Connection> get_by_key(
            const Key& key,
            bool required);

        static std::shared_ptr<Connection> get_by_id(
            const std::string& id,
            bool required);

    protected:
        void initialize() override;
        void set_ready() override;
        void get_settings();
        void on_signal_updated(const Glib::VariantContainerBase& parameters);
        void emit_change(core::signal::MappingAction action) override;

    public:
        cc::dbus::ObjectPath find_suitable_device();
        void replace(const ConnectionData& connection);
        void remove();
        void remove_duplicates();

    public:
        ConnectionData data;
        // Glib::VariantContainerBase secrets;
    };

}  // namespace cc::platform::netconfig::dbus
