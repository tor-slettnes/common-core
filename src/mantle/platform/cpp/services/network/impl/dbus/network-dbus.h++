// -*- c++ -*-
//==============================================================================
/// @file network-provider-dbus.h++
/// @brief Network service - D-BUS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network-provider.h++"
#include "nm-manager.h++"

namespace platform::network::dbus
{
    //======================================================================
    /// Abstract base for network providers

    class DBusProvider : public network::Provider
    {
        using This = DBusProvider;
        using Super = network::Provider;

    protected:
        DBusProvider();

    public:
        bool is_pertinent() override;
        void initialize() override;

    public:
        //======================================================================
        // Settings methods

        std::string get_hostname() const override;
        void set_hostname(const std::string &hostname) override;

        //======================================================================
        // Connection methods

        ConnectionMap get_connections() const override;
        void define_connection(const ConnectionData &connection,
                               bool activate) override;
        bool remove_connection(const Key &key) override;
        void activate_connection(const Key &key) override;
        void deactivate_connection(const Key &key) override;

        //======================================================================
        // ActiveConnection methods

        ActiveConnectionMap get_active_connections() const override;

        //======================================================================
        // AccessPoint methods

        void request_scan() override;
        AccessPointMap get_aps() const override;
        void connect_ap(const Key &bssid,
                        const ConnectionData &connection) override;
        void connect_ap(const SSID &ssid,
                        const ConnectionData &connection) override;

        //======================================================================
        // Device methods

        DeviceMap get_devices() const override;

        //======================================================================
        // Manager methods

        GlobalDataRef get_global_data() const override;
        void set_wireless_enabled(bool enabled) override;
        void set_wireless_allowed(bool allowed) override;
        void select_wireless_band(WirelessBandSelection band) override;

    private:
        std::shared_ptr<Manager> manager;
        std::shared_ptr<Settings> settings;
    };
}  // namespace platform::network::dbus
