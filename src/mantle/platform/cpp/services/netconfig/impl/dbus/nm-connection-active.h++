/// -*- c++ -*-
//==============================================================================
/// @file nm-connection-active.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace netconfig::dbus
{
    //==========================================================================
    /// Network Manager Active Connection

    class ActiveConnection : public MappedDataWrapper<ActiveConnectionData>
    {
        using Class = ActiveConnection;
        using Super = MappedDataWrapper<ActiveConnectionData>;

    public:
        ActiveConnection(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath);

        static std::shared_ptr<ActiveConnection> get_by_key(
            const Key& key,
            bool required);

        static std::shared_ptr<ActiveConnection> get_by_id(
            const std::string& id,
            bool required);

    protected:
        void emit_change(core::signal::MappingAction action) override;
        void on_signal_state_changed(const Glib::VariantContainerBase& parameters);
        void on_property_type(const Glib::VariantBase& change);
        // void on_property_uuid(const Glib::VariantBase& change);
        void on_property_ip4config(const Glib::VariantBase& change);
        void on_property_ip6config(const Glib::VariantBase& change);

        bool update_ip4config(const core::dbus::ProxyWrapper* source, core::signal::MappingAction action);
        bool update_ip6config(const core::dbus::ProxyWrapper* source, core::signal::MappingAction action);
    };

}  // namespace netconfig::dbus
