// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-provider.c++
/// @brief NetConfig service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-grpc-provider.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-netconfig-types.h++"
#include "protobuf-inline.h++"
#include "thread/signaltemplate.h++"

namespace netconfig::grpc
{
    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client,
                                   bool use_cached)
        : Super("netconfig::ClientProvider"),
          client(client),
          use_cached(use_cached)
    {
    }

    bool ClientProvider::is_pertinent() const
    {
        return bool(client);
    }

    void ClientProvider::initialize()
    {
        Provider::initialize();
        using namespace std::placeholders;

        this->client->add_mapping_handler(
            ::cc::platform::netconfig::protobuf::Signal::kConnection,
            [](core::signal::MappingAction action,
               const std::string &key,
               const ::cc::platform::netconfig::protobuf::Signal &signal) {
                netconfig::signal_connection.emit(
                    action,
                    key,
                    protobuf::decoded_shared<ConnectionData>(signal.connection()));
            });

        this->client->add_mapping_handler(
            ::cc::platform::netconfig::protobuf::Signal::kActiveConnection,
            [](core::signal::MappingAction action,
               const std::string &key,
               const ::cc::platform::netconfig::protobuf::Signal &signal) {
                netconfig::signal_active_connection.emit(
                    action,
                    key,
                    protobuf::decoded_shared<ActiveConnectionData>(signal.active_connection()));
            });

        this->client->add_mapping_handler(
            ::cc::platform::netconfig::protobuf::Signal::kAccesspoint,
            [](core::signal::MappingAction action,
               const std::string &key,
               const ::cc::platform::netconfig::protobuf::Signal &signal) {
                netconfig::signal_accesspoint.emit(
                    action,
                    key,
                    protobuf::decoded_shared<AccessPointData>(signal.accesspoint()));
            });

        this->client->add_mapping_handler(
            ::cc::platform::netconfig::protobuf::Signal::kDevice,
            [](core::signal::MappingAction action,
               const std::string &key,
               const ::cc::platform::netconfig::protobuf::Signal &signal) {
                netconfig::signal_device.emit(
                    action,
                    key,
                    protobuf::decoded_shared<DeviceData>(signal.device()));
            });

        this->client->add_handler(
            ::cc::platform::netconfig::protobuf::Signal::kGlobal,
            [](const ::cc::platform::netconfig::protobuf::Signal &signal) {
                netconfig::signal_globaldata.emit(
                    protobuf::decoded_shared<GlobalData>(signal.global()));
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
            this->client->call_check(&Client::Stub::GetHostName));
    }

    void ClientProvider::set_hostname(const std::string &hostname)
    {
        this->client->call_check(
            &Client::Stub::SetHostName,
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
                this->client->call_check(&Client::Stub::GetConnections));
        }
    }

    void ClientProvider::define_connection(
        const ConnectionData &connection,
        bool activate)
    {
        ::cc::platform::netconfig::protobuf::ConnectionRequest request;
        protobuf::encode(connection, request.mutable_data());
        request.set_activate(activate);
        this->client->call_check(&Client::Stub::DefineConnection, request);
    }

    bool ClientProvider::remove_connection(const Key &key)
    {
        ::cc::platform::netconfig::protobuf::MappingKey req;
        req.set_key(key);
        return protobuf::decoded<bool>(
            this->client->call_check(
                &Client::Stub::RemoveConnection, req));
    }

    void ClientProvider::activate_connection(const Key &key)
    {
        ::cc::platform::netconfig::protobuf::MappingKey req;
        req.set_key(key);
        this->client->call_check(&Client::Stub::ActivateConnection, req);
    }

    void ClientProvider::deactivate_connection(const Key &key)
    {
        ::cc::platform::netconfig::protobuf::MappingKey req;
        req.set_key(key);
        this->client->call_check(&Client::Stub::DeactivateConnection, req);
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
                this->client->call_check(&Client::Stub::GetActiveConnections));
        }
    }

    //==========================================================================
    // AccessPoint Methods

    void ClientProvider::request_scan()
    {
        this->client->call_check(&Client::Stub::RequestScan);
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
                this->client->call_check(&Client::Stub::GetAccessPoints));
        }
    }

    void ClientProvider::connect_ap(const Key &bssid,
                                    const ConnectionData &connection)
    {
        ::cc::platform::netconfig::protobuf::WirelessConnectionRequest req;
        req.set_bssid(bssid);
        protobuf::encode(connection, req.mutable_connection());
        this->client->call_check(&Client::Stub::ConnectAccessPoint, req);
    }

    void ClientProvider::connect_ap(const core::types::ByteVector &ssid,
                                    const ConnectionData &connection)
    {
        ::cc::platform::netconfig::protobuf::WirelessConnectionRequest req;
        req.set_ssid(ssid.data(), ssid.size());
        protobuf::encode(connection, req.mutable_connection());
        this->client->call_check(&Client::Stub::ConnectAccessPoint, req);
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
                this->client->call_check(&Client::Stub::GetDevices));
        }
    }

    //======================================================================
    // Manager methods

    GlobalData::ptr ClientProvider::get_global_data() const
    {
        if (this->get_use_cached())
        {
            return signal_globaldata.get_cached().value_or(GlobalData::ptr());
        }
        else
        {
            auto ref = std::make_shared<GlobalData>();
            protobuf::decode(
                this->client->call_check(&Client::Stub::GetGlobalData),
                ref.get());
            return ref;
        }
    }

    void ClientProvider::set_wireless_enabled(bool enabled)
    {
        ::cc::platform::netconfig::protobuf::RadioState req;
        req.set_wireless_enabled(enabled);
        logf_debug("Setting wireless radio switch: %b", enabled);
        this->client->call_check(&Client::Stub::SetWirelessEnabled, req);
    }

    void ClientProvider::set_wireless_allowed(bool allowed)
    {
        logf_debug("Setting wireless allowed flag: %b", allowed);
        this->client->call_check(
            &Client::Stub::SetWirelessAllowed,
            protobuf::encoded<google::protobuf::BoolValue>(allowed));
    }

    void ClientProvider::select_wireless_band(WirelessBandSelection band_selection)
    {
        logf_debug("Selecting wireless band: %s", band_selection);
        ::cc::platform::netconfig::protobuf::WirelessBandSetting req;
        req.set_band_selection(
            protobuf::encoded<::cc::platform::netconfig::protobuf::WirelessBandSelection>(band_selection));

        this->client->call_check(&Client::Stub::SelectWirelessBand, req);
    }

}  // namespace netconfig::grpc
