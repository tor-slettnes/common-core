// -*- c++ -*-
//==============================================================================
/// @file network-grpc-provider.c++
/// @brief Network service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-grpc-provider.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-network.h++"
#include "protobuf-inline.h++"
#include "thread/signaltemplate.h++"

namespace platform::network::grpc
{
    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client,
                                   bool use_cached)
        : Super("network::ClientProvider"),
          client(client),
          use_cached(use_cached)
    {
    }

    bool ClientProvider::is_pertinent()
    {
        return bool(client);
    }

    void ClientProvider::initialize()
    {
        Provider::initialize();
        using namespace std::placeholders;

        this->client->add_mapping_handler(
            cc::platform::network::Signal::kConnection,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::network::Signal &signal) {
                platform::network::signal_connection.emit(
                    action,
                    key,
                    protobuf::decode_shared<ConnectionData>(signal.connection()));
            });

        this->client->add_mapping_handler(
            cc::platform::network::Signal::kActiveConnection,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::network::Signal &signal) {
                platform::network::signal_active_connection.emit(
                    action,
                    key,
                    protobuf::decode_shared<ActiveConnectionData>(signal.active_connection()));
            });

        this->client->add_mapping_handler(
            cc::platform::network::Signal::kAccesspoint,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::network::Signal &signal) {
                platform::network::signal_accesspoint.emit(
                    action,
                    key,
                    protobuf::decode_shared<AccessPointData>(signal.accesspoint()));
            });

        this->client->add_mapping_handler(
            cc::platform::network::Signal::kDevice,
            [](core::signal::MappingAction action,
               const std::string &key,
               const cc::platform::network::Signal &signal) {
                platform::network::signal_device.emit(
                    action,
                    key,
                    protobuf::decode_shared<DeviceData>(signal.device()));
            });

        this->client->add_handler(
            cc::platform::network::Signal::kGlobal,
            [](const cc::platform::network::Signal &signal) {
                platform::network::signal_globaldata.emit(
                    protobuf::decode_shared<GlobalData>(signal.global()));
            });
    }

    void ClientProvider::set_use_cached(bool use_cached)
    {
        this->use_cached = use_cached;
    }

    bool ClientProvider::get_use_cached() const
    {
        return this->use_cached;
    }

    //======================================================================
    // Settings methods

    std::string ClientProvider::get_hostname() const
    {
        return protobuf::decoded<std::string>(
            this->client->call_check(&Client::Stub::get_hostname));
    }

    void ClientProvider::set_hostname(const std::string &hostname)
    {
        this->client->call_check(
            &Client::Stub::set_hostname,
            protobuf::encoded<protobuf::StringValue>(hostname));
    }

    //==========================================================================
    // Connection methods

    ConnectionMap ClientProvider::get_connections() const
    {
        if (this->get_use_cached())
        {
            return signal_connection.get_cached();
        }
        else
        {
            return protobuf::decoded<ConnectionMap>(
                this->client->call_check(&Client::Stub::get_connections));
        }
    }

    void ClientProvider::define_connection(
        const ConnectionData &connection,
        bool activate)
    {
        cc::platform::network::ConnectionRequest request;
        protobuf::encode(connection, request.mutable_data());
        request.set_activate(activate);
        this->client->call_check(&Client::Stub::define_connection, request);
    }

    bool ClientProvider::remove_connection(const Key &key)
    {
        cc::platform::network::MappingKey req;
        req.set_key(key);
        return protobuf::decoded<bool>(
            this->client->call_check(
                &Client::Stub::remove_connection, req));
    }

    void ClientProvider::activate_connection(const Key &key)
    {
        cc::platform::network::MappingKey req;
        req.set_key(key);
        this->client->call_check(&Client::Stub::activate_connection, req);
    }

    void ClientProvider::deactivate_connection(const Key &key)
    {
        cc::platform::network::MappingKey req;
        req.set_key(key);
        this->client->call_check(&Client::Stub::deactivate_connection, req);
    }

    //==========================================================================
    // ActiveConnection Methods

    ActiveConnectionMap ClientProvider::get_active_connections() const
    {
        if (this->get_use_cached())
        {
            return signal_active_connection.get_cached();
        }
        else
        {
            return protobuf::decoded<ActiveConnectionMap>(
                this->client->call_check(&Client::Stub::get_active_connections));
        }
    }

    //==========================================================================
    // AccessPoint Methods

    void ClientProvider::request_scan()
    {
        this->client->call_check(&Client::Stub::request_scan);
    }

    AccessPointMap ClientProvider::get_aps() const
    {
        if (this->get_use_cached())
        {
            return signal_accesspoint.get_cached();
        }
        else
        {
            return protobuf::decoded<AccessPointMap>(
                this->client->call_check(&Client::Stub::get_aps));
        }
    }

    void ClientProvider::connect_ap(const Key &bssid,
                                    const ConnectionData &connection)
    {
        cc::platform::network::AccessPointConnection req;
        req.set_bssid(bssid);
        protobuf::encode(connection, req.mutable_connection());
        this->client->call_check(&Client::Stub::connect_ap, req);
    }

    void ClientProvider::connect_ap(const core::types::ByteVector &ssid,
                                    const ConnectionData &connection)
    {
        cc::platform::network::AccessPointConnection req;
        req.set_ssid(ssid.data(), ssid.size());
        protobuf::encode(connection, req.mutable_connection());
        this->client->call_check(&Client::Stub::connect_ap, req);
    }

    //==========================================================================
    // Device Methods

    DeviceMap ClientProvider::get_devices() const
    {
        if (this->get_use_cached())
        {
            return signal_device.get_cached();
        }
        else
        {
            return protobuf::decoded<DeviceMap>(
                this->client->call_check(&Client::Stub::get_devices));
        }
    }

    //======================================================================
    // Manager methods

    GlobalDataRef ClientProvider::get_global_data() const
    {
        if (this->get_use_cached())
        {
            return signal_globaldata.get_cached().value_or(GlobalDataRef());
        }
        else
        {
            auto ref = std::make_shared<GlobalData>();
            protobuf::decode(
                this->client->call_check(&Client::Stub::get_global_data),
                ref.get());
            return ref;
        }
    }

    void ClientProvider::set_wireless_enabled(bool enabled)
    {
        cc::platform::network::RadioState req;
        req.set_wireless_enabled(enabled);
        logf_debug("Setting wireless radio switch: %b", enabled);
        this->client->call_check(&Client::Stub::set_wireless_enabled, req);
    }

    void ClientProvider::set_wireless_allowed(bool allowed)
    {
        logf_debug("Setting wireless allowed flag: %b", allowed);
        this->client->call_check(
            &Client::Stub::set_wireless_allowed,
            protobuf::encoded<google::protobuf::BoolValue>(allowed));
    }

    void ClientProvider::select_wireless_band(WirelessBandSelection band_selection)
    {
        logf_debug("Selecting wireless band: %s", band_selection);
        cc::platform::network::WirelessBandSetting req;
        req.set_band_selection(
            protobuf::encoded<cc::platform::network::WirelessBandSelection>(band_selection));

        this->client->call_check(&Client::Stub::select_wireless_band, req);
    }

}  // namespace platform::network::grpc
