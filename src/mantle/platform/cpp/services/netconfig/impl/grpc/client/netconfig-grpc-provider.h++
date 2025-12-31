// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-provider.h++
/// @brief NetConfig service - gRPC client implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "netconfig.h++"
#include "netconfig-grpc-client.h++"

namespace netconfig::grpc
{
    class ClientProvider : public ProviderInterface
    {
        using This = ClientProvider;
        using Super = ProviderInterface;

    public:
        ClientProvider(const std::shared_ptr<Client> &client,
                       bool use_cached = false);

        bool is_pertinent() const override;
        void initialize() override;

        void set_use_cached(bool use_cached);
        bool get_use_cached() const;

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

        GlobalData::ptr get_global_data() const override;
        void set_wireless_enabled(bool enabled) override;
        void set_wireless_allowed(bool allowed) override;
        void select_wireless_band(WirelessBandSelection band) override;

    private:
        std::shared_ptr<Client> client;
        bool use_cached;
    };
}  // namespace netconfig::grpc
