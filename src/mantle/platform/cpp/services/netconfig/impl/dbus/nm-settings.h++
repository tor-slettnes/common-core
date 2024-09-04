/// -*- c++ -*-
//==============================================================================
/// @file nm-settings.h++
/// @brief Specialized Gio::DBus::Proxy classes for NetConfig
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "nm-wrappers.h++"

namespace platform::netconfig::dbus
{
    //==========================================================================
    /// Network Manager Settings

    class Settings : public DataWrapper<SystemData>
    {
        using Class = Settings;
        using Super = DataWrapper<SystemData>;

    public:
        Settings(
            core::dbus::ProxyContainer* container,
            const core::dbus::ConnectionPtr& connection,
            const core::dbus::ServiceName& servicename,
            const core::dbus::ObjectPath& objectpath = NM_DBUS_PATH_SETTINGS);

        void define_connection(const ConnectionData& connection);
        bool remove_connection(const std::string& id, bool required = false);
        void set_hostname(const std::string& hostname);

    private:
        void add_connection(const ConnectionData& connection);

    protected:
        void initialize_properties() override;
        void on_signal_connection_added(const Glib::VariantContainerBase& parameters);
        void on_signal_connection_removed(const Glib::VariantContainerBase& parameters);
        void on_property_hostname(const Glib::VariantBase& change);
    };

}  // namespace platform::netconfig::dbus
