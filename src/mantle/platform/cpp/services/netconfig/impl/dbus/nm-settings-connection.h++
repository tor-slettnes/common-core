/// -*- c++ -*-
//==============================================================================
/// @file nm-settings-connection.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"
#include "nm-structs.h++"
#include "types/bytevector.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager Connection Settings

    class Connection : public MappedDataWrapper<ConnectionData>
    {
        using Class = Connection;
        using Super = MappedDataWrapper<ConnectionData>;

    public:
        Connection(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

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
        core::dbus::ObjectPath find_suitable_device();
        void replace(const ConnectionData& connection);
        void remove();
        void remove_duplicates();

    public:
        ConnectionData data;
        // Glib::VariantContainerBase secrets;
    };

}  // namespace platform::netconfig::dbus
