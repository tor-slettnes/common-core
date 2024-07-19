// -*- c++ -*-
//==============================================================================
/// @file network-provider.h++
/// @brief Network service - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network-types.h++"
#include "network-signals.h++"

namespace platform::network
{
    //======================================================================
    /// Abstract base for network providers

    class Provider : public core::platform::Provider
    {
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //======================================================================
        // Settings methods

        virtual std::string get_hostname() const = 0;
        virtual void set_hostname(const std::string &hostname) = 0;

        //======================================================================
        // Connection methods

        ConnectionRef get_connection(const Key &key) const;
        ConnectionRef get_connection_by_ssid(const SSID &ssid) const;

        virtual ConnectionMap get_connections() const = 0;
        virtual void define_connection(const ConnectionData &connection,
                                       bool activate) = 0;
        virtual bool remove_connection(const Key &key) = 0;
        virtual void activate_connection(const Key &key) = 0;
        virtual void deactivate_connection(const Key &key) = 0;

        //======================================================================
        // ActiveConnection methods

        ActiveConnectionRef get_active_connection(const Key &key) const;
        ActiveConnectionRef get_active_connection(ConnectionType type) const;
        virtual ActiveConnectionMap get_active_connections() const = 0;

        //======================================================================
        // AccessPoint methods

        AccessPointRef get_ap(const Key &key) const;
        SSIDMap get_aps_by_ssid() const;
        std::vector<AccessPointRef> get_aps_by_strongest_ssid() const;
        AccessPointRef get_active_ap() const;
        AccessPointRef get_active_ap(const DeviceRef &dev) const;
        std::set<SSID> get_active_ssids() const;
        bool ap_supported(AccessPointRef ap) const;

        virtual void request_scan() = 0;
        virtual AccessPointMap get_aps() const = 0;
        virtual void connect_ap(const Key &bssid,
                                const ConnectionData &connection) = 0;
        virtual void connect_ap(const SSID &ssid,
                                const ConnectionData &connection) = 0;

        //======================================================================
        // Device methods

        DeviceRef get_device(const Key &key) const;
        DeviceRef get_device(NMDeviceType type) const;
        virtual DeviceMap get_devices() const = 0;

        //======================================================================
        // Manager methods

        WifiTuple get_active_wifi_data() const;
        bool has_gateway() const;
        NMConnectivityState get_connectivity() const;

        virtual GlobalDataRef get_global_data() const = 0;
        virtual void set_wireless_enabled(bool enabled) = 0;
        virtual void set_wireless_allowed(bool allowed) = 0;
        virtual void select_wireless_band(WirelessBandSelection band) = 0;
    };

    extern core::platform::ProviderProxy<Provider> network;
}  // namespace platform::network
