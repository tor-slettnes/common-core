// -*- c++ -*-
//==============================================================================
/// @file netconfig.h++
/// @brief NetConfig service - abstract interface
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "netconfig-types.h++"
#include "netconfig-signals.h++"
#include "platform/provider.h++"

namespace netconfig
{
    //======================================================================
    /// Abstract base for network providers

    class ProviderInterface : public core::platform::Provider
    {
        using This = ProviderInterface;
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

        ConnectionData::ptr get_connection(const Key &key) const;
        ConnectionData::ptr get_connection_by_ssid(const SSID &ssid) const;

        virtual ConnectionMap get_connections() const = 0;
        virtual void define_connection(const ConnectionData &connection,
                                       bool activate) = 0;
        virtual bool remove_connection(const Key &key) = 0;
        virtual void activate_connection(const Key &key) = 0;
        virtual void deactivate_connection(const Key &key) = 0;

        //======================================================================
        // ActiveConnection methods

        ActiveConnectionData::ptr get_active_connection(const Key &key) const;
        ActiveConnectionData::ptr get_active_connection(ConnectionType type) const;
        virtual ActiveConnectionMap get_active_connections() const = 0;

        //======================================================================
        // AccessPoint methods

        AccessPointData::ptr get_ap(const Key &key) const;
        SSIDMap get_aps_by_ssid() const;
        std::vector<AccessPointData::ptr> get_aps_by_strongest_ssid() const;
        AccessPointData::ptr get_active_ap() const;
        AccessPointData::ptr get_active_ap(const DeviceData::ptr &dev) const;
        std::set<SSID> get_active_ssids() const;
        bool ap_supported(AccessPointData::ptr ap) const;

        virtual void request_scan() = 0;
        virtual AccessPointMap get_aps() const = 0;
        virtual void connect_ap(const Key &bssid,
                                const ConnectionData &connection) = 0;
        virtual void connect_ap(const SSID &ssid,
                                const ConnectionData &connection) = 0;

        //======================================================================
        // Device methods

        DeviceData::ptr get_device(const Key &key) const;
        DeviceData::ptr get_device(NMDeviceType type) const;
        virtual DeviceMap get_devices() const = 0;

        //======================================================================
        // Manager methods

        WifiTuple get_active_wifi_data() const;
        bool has_gateway() const;
        NMConnectivityState get_connectivity() const;

        virtual GlobalData::ptr get_global_data() const = 0;
        virtual void set_wireless_enabled(bool enabled) = 0;
        virtual void set_wireless_allowed(bool allowed) = 0;
        virtual void select_wireless_band(WirelessBandSelection band) = 0;
    };

    extern core::platform::ProviderProxy<ProviderInterface> network;
}  // namespace netconfig
