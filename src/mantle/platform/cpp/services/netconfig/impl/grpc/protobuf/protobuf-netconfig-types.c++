/// -*- c++ -*-
//==============================================================================
/// @file protobuf-netconfig.c++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-netconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // ::platform::netconfig::GlobalData

    void encode(const ::platform::netconfig::GlobalData &data,
                ::cc::platform::netconfig::GlobalData *msg) noexcept
    {
        msg->set_state(encoded<::cc::platform::netconfig::NetworkState>(data.state));
        msg->set_connectivity(
            encoded<::cc::platform::netconfig::ConnectivityState>(data.connectivity));
        msg->set_wireless_hardware_enabled(data.wireless_hardware_enabled);
        msg->set_wireless_enabled(data.wireless_enabled);
        msg->set_wireless_allowed(data.wireless_allowed);
        msg->set_wireless_band_selection(
            encoded<::cc::platform::netconfig::WirelessBandSelection>(data.wireless_band_selection));
    }

    void decode(const ::cc::platform::netconfig::GlobalData &msg,
                ::platform::netconfig::GlobalData *data) noexcept
    {
        data->state = decoded<NMState>(msg.state());
        data->connectivity = decoded<NMConnectivityState>(msg.connectivity());
        data->wireless_hardware_enabled = msg.wireless_hardware_enabled();
        data->wireless_enabled = msg.wireless_enabled();
        data->wireless_allowed = msg.wireless_allowed();
        data->wireless_band_selection =
            decoded<::platform::netconfig::WirelessBandSelection>(msg.wireless_band_selection());
    }

    //==========================================================================
    // ::platform::netconfig::IPConfig

    void encode(const ::platform::netconfig::IPConfigData &config,
                ::cc::platform::netconfig::IPConfigData *msg) noexcept
    {
        msg->set_method(encoded<::cc::platform::netconfig::IPConfigMethod>(config.method));

        for (const ::platform::netconfig::AddressData &data : config.address_data)
        {
            encode(data, msg->add_address_data());
        }
        msg->set_gateway(config.gateway);
        for (const ::platform::netconfig::IPAddress &dns : config.dns)
        {
            msg->add_dns(dns);
        }
        for (const ::platform::netconfig::Domain &search : config.searches)
        {
            msg->add_searches(search);
        }
    }

    void decode(const ::cc::platform::netconfig::IPConfigData &msg,
                ::platform::netconfig::IPConfigData *config) noexcept
    {
        config->method = decoded<::platform::netconfig::IPConfigMethod>(msg.method());
        config->address_data.clear();
        config->gateway = msg.gateway();
        config->dns.clear();
        config->searches.clear();

        for (const auto &data : msg.address_data())
        {
            decode(data, &config->address_data.emplace_back());
        }
        for (const auto &address : msg.dns())
        {
            config->dns.push_back(address);
        }
        for (const auto &search : msg.searches())
        {
            config->searches.push_back(search);
        }
    }

    //==========================================================================
    // ::platform::netconfig::IPConfigMethod

    void encode(::platform::netconfig::IPConfigMethod method,
                ::cc::platform::netconfig::IPConfigMethod *protomethod) noexcept
    {
        *protomethod = static_cast<::cc::platform::netconfig::IPConfigMethod>(method);
    }

    void decode(::cc::platform::netconfig::IPConfigMethod protomethod,
                ::platform::netconfig::IPConfigMethod *method) noexcept
    {
        *method = static_cast<::platform::netconfig::IPConfigMethod>(protomethod);
    }

    //==========================================================================
    // ::platform::netconfig::AddressData

    void encode(const ::platform::netconfig::AddressData &data,
                ::cc::platform::netconfig::AddressData *msg) noexcept
    {
        msg->set_address(data.address);
        msg->set_prefixlength(data.prefixlength);
    }

    void decode(const ::cc::platform::netconfig::AddressData &msg,
                ::platform::netconfig::AddressData *data) noexcept
    {
        data->address = msg.address();
        data->prefixlength = msg.prefixlength();
    }

    //==========================================================================
    // ::platform::netconfig::ConnectionMap

    void encode(const ::platform::netconfig::ConnectionMap &map,
                ::cc::platform::netconfig::ConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::ConnectionMap &msg,
                ::platform::netconfig::ConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::netconfig::ConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::netconfig::ConnectionData

    void encode(const ::platform::netconfig::ConnectionData &data,
                ::cc::platform::netconfig::ConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_interface(data.interface);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());

        switch (data.type())
        {
        case ::platform::netconfig::CONN_TYPE_WIRED:
            encode(std::get<::platform::netconfig::WiredConnectionData>(data.specific_data),
                   msg->mutable_wired_data());
            break;

        case ::platform::netconfig::CONN_TYPE_WIRELESS:
            encode(std::get<::platform::netconfig::WirelessConnectionData>(data.specific_data),
                   msg->mutable_wireless_data());
            break;

        default:
            break;
        }
    }

    void decode(const ::cc::platform::netconfig::ConnectionData &msg,
                ::platform::netconfig::ConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->interface = msg.interface();
        data->uuid = msg.uuid();

        decode(msg.ip4config(), &data->ip4config);
        decode(msg.ip6config(), &data->ip6config);

        switch (msg.data_case())
        {
        case ::cc::platform::netconfig::ConnectionData::DataCase::kWiredData:
            data->specific_data =
                decoded<::platform::netconfig::WiredConnectionData>(msg.wired_data());
            break;

        case ::cc::platform::netconfig::ConnectionData::DataCase::kWirelessData:
            data->specific_data =
                decoded<::platform::netconfig::WirelessConnectionData>(msg.wireless_data());
            break;

        default:
            data->specific_data = {};
            break;
        }
    }

    //==========================================================================
    // ::platform::netconfig::WiredConnectionData

    void encode(const ::platform::netconfig::WiredConnectionData &data,
                ::cc::platform::netconfig::WiredConnectionData *msg) noexcept
    {
        msg->set_auto_negotiate(data.auto_negotiate);
    }

    void decode(const ::cc::platform::netconfig::WiredConnectionData &msg,
                ::platform::netconfig::WiredConnectionData *data) noexcept
    {
        data->auto_negotiate = msg.auto_negotiate();
    }

    //==========================================================================
    // ::platform::netconfig::WirelessConnectionData

    void encode(const ::platform::netconfig::WirelessConnectionData &data,
                ::cc::platform::netconfig::WirelessConnectionData *msg) noexcept
    {
        msg->set_ssid(data.ssid.data(), data.ssid.size());
        msg->set_mode(encoded<::cc::platform::netconfig::WirelessMode>(data.mode));
        msg->set_hidden(data.hidden);
        msg->set_tx_power(data.tx_power);
        msg->set_key_mgmt(encoded<::cc::platform::netconfig::KeyManagement>(data.key_mgmt_type()));
        msg->set_auth_protos(data.auth_protos);
        msg->set_auth_type(encoded<::cc::platform::netconfig::AuthenticationType>(data.auth_type()));

        if (auto *wep = std::get_if<::platform::netconfig::WEP_Data>(&data.auth))
        {
            encode(*wep, msg->mutable_wep());
        }
        else if (auto *wpa = std::get_if<::platform::netconfig::WPA_Data>(&data.auth))
        {
            encode(*wpa, msg->mutable_wpa());
        }
        else if (auto *eap = std::get_if<::platform::netconfig::EAP_Data>(&data.auth))
        {
            encode(*eap, msg->mutable_eap());
        }

        if (data.band)
        {
            msg->set_band(encoded<::cc::platform::netconfig::WirelessBandSelection>(*data.band));
        }
    }

    void decode(const ::cc::platform::netconfig::WirelessConnectionData &msg,
                ::platform::netconfig::WirelessConnectionData *data) noexcept
    {
        data->ssid = msg.ssid();
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->hidden = msg.hidden();
        data->tx_power = msg.tx_power();
        data->key_mgmt = decoded<::platform::netconfig::KeyManagement>(msg.key_mgmt());
        data->auth_protos = msg.auth_protos();

        switch (msg.auth_case())
        {
        case ::cc::platform::netconfig::WirelessConnectionData::AuthCase::kWep:
            data->auth = decoded<::platform::netconfig::WEP_Data>(msg.wep());
            break;

        case ::cc::platform::netconfig::WirelessConnectionData::AuthCase::kWpa:
            data->auth = decoded<::platform::netconfig::WPA_Data>(msg.wpa());
            break;

        case ::cc::platform::netconfig::WirelessConnectionData::AuthCase::kEap:
            data->auth = decoded<::platform::netconfig::EAP_Data>(msg.eap());
            break;

        default:
            data->auth = {};
            break;
        }

        if (msg.optional_band_case())
        {
            data->band = decoded<::platform::netconfig::WirelessBandSelection>(msg.band());
        }
    }

    //==========================================================================
    // ::platform::netconfig::ActiveConnectionMap

    void encode(const ::platform::netconfig::ActiveConnectionMap &map,
                ::cc::platform::netconfig::ActiveConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::ActiveConnectionMap &msg,
                ::platform::netconfig::ActiveConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::netconfig::ActiveConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::netconfig::ActiveConnectionData

    void encode(const ::platform::netconfig::ActiveConnectionData &data,
                ::cc::platform::netconfig::ActiveConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_type(encoded<::cc::platform::netconfig::ConnectionType>(data.type));
        msg->set_state(encoded<::cc::platform::netconfig::ActiveConnectionState>(data.state));
        msg->set_state_reason(
            encoded<::cc::platform::netconfig::ActiveConnectionStateReason>(data.state_reason));

        msg->set_state_flags(data.state_flags);
        msg->set_default4(data.default4);
        msg->set_default6(data.default6);
        msg->set_vpn(data.vpn);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());
    }

    void decode(const ::cc::platform::netconfig::ActiveConnectionData &msg,
                ::platform::netconfig::ActiveConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->type = decoded<::platform::netconfig::ConnectionType>(msg.type());
        data->state = decoded<NMActiveConnectionState>(msg.state());
        data->state_reason = decoded<NMActiveConnectionStateReason>(msg.state_reason());
        data->state_flags = msg.state_flags();
        data->default4 = msg.default4();
        data->default6 = msg.default6();
        data->vpn = msg.vpn();
        data->uuid = msg.uuid();

        decode(msg.ip4config(), &data->ip4config);
        decode(msg.ip6config(), &data->ip6config);
    }

    //==========================================================================
    // ::platform::netconfig::WEP_Data

    void encode(const ::platform::netconfig::WEP_Data &auth,
                ::cc::platform::netconfig::WEP_Data *msg) noexcept
    {
        msg->set_auth_alg(encoded<::cc::platform::netconfig::AuthenticationAlgorithm>(auth.auth_alg));
        for (const core::types::ByteVector &key : auth.keys)
        {
            msg->add_key(key.data(), key.size());
        }
        msg->set_key_idx(auth.key_idx);
        msg->set_key_type(encoded<::cc::platform::netconfig::WEP_KeyType>(auth.key_type));
    }

    void decode(const ::cc::platform::netconfig::WEP_Data &msg,
                ::platform::netconfig::WEP_Data *auth) noexcept
    {
        auth->auth_alg =
            decoded<::platform::netconfig::AuthenticationAlgorithm>(msg.auth_alg());

        auth->keys.clear();
        auth->keys.reserve(msg.key_size());
        for (const auto &key : msg.key())
        {
            auth->keys.push_back(key);
        }
        auth->keys.resize(WEP_KEY_COUNT);

        if ((msg.key_idx() >= 0) && (msg.key_idx() < WEP_KEY_COUNT))
        {
            auth->key_idx = msg.key_idx();
        }
        else
        {
            auth->key_idx = 0;
        }
        auth->key_type = decoded<NMWepKeyType>(msg.key_type());
    }

    //==========================================================================
    // ::platform::netconfig::WPA_Data

    void encode(const ::platform::netconfig::WPA_Data &auth,
                ::cc::platform::netconfig::WPA_Data *msg) noexcept
    {
        msg->set_psk(auth.psk);
    }

    void decode(const ::cc::platform::netconfig::WPA_Data &msg,
                ::platform::netconfig::WPA_Data *auth) noexcept
    {
        auth->psk = msg.psk();
    }

    //==========================================================================
    // ::platform::netconfig::EAP_Data

    void encode(const ::platform::netconfig::EAP_Data &auth,
                ::cc::platform::netconfig::EAP_Data *msg) noexcept
    {
        msg->set_auth_alg(
            encoded<::cc::platform::netconfig::AuthenticationAlgorithm>(
                auth.auth_alg));

        msg->set_eap_type(encoded<::cc::platform::netconfig::EAP_Type>(auth.eap_type));
        msg->set_eap_phase2(encoded<::cc::platform::netconfig::EAP_Phase2>(auth.eap_phase2));
        msg->set_anonymous_identity(auth.anonymous_identity);
        msg->set_domain(auth.domain);
        msg->set_identity(auth.identity);
        msg->set_password(auth.password);
        msg->set_ca_cert(auth.ca_cert.string());
        msg->set_ca_cert_password(auth.ca_cert_password);
        msg->set_client_cert(auth.client_cert.string());
        msg->set_client_cert_key(auth.client_cert_key.string());
        msg->set_client_cert_password(auth.client_cert_password);
        msg->set_pac_file(auth.pac_file.string());
        msg->set_fast_provisioning(
            encoded<::cc::platform::netconfig::FAST_Provisioning>(auth.fast_provisioning));
    }

    void decode(const ::cc::platform::netconfig::EAP_Data &msg,
                ::platform::netconfig::EAP_Data *auth) noexcept
    {
        auth->auth_alg =
            decoded<::platform::netconfig::AuthenticationAlgorithm>(msg.auth_alg());

        auth->eap_type = decoded<::platform::netconfig::EAP_Type>(msg.eap_type());
        auth->eap_phase2 = decoded<::platform::netconfig::EAP_Phase2>(msg.eap_phase2());
        auth->anonymous_identity = msg.anonymous_identity();
        auth->domain = msg.domain();
        auth->identity = msg.identity();
        auth->password = msg.password();
        auth->ca_cert = msg.ca_cert();
        auth->ca_cert_password = msg.ca_cert_password();
        auth->client_cert = msg.client_cert();
        auth->client_cert_key = msg.client_cert_key();
        auth->client_cert_password = msg.client_cert_password();
        auth->pac_file = msg.pac_file();
        auth->fast_provisioning =
            decoded<::platform::netconfig::FAST_Provisioning>(msg.fast_provisioning());
    }

    //==========================================================================
    // ::platform::netconfig::Device

    void encode(const ::platform::netconfig::DeviceData &device,
                ::cc::platform::netconfig::DeviceData *msg) noexcept
    {
        msg->set_type(encoded<::cc::platform::netconfig::DeviceType>(device.type));
        msg->set_state(encoded<::cc::platform::netconfig::DeviceState>(device.state));
        msg->set_state_reason(
            encoded<::cc::platform::netconfig::DeviceStateReason>(device.state_reason));
        msg->set_flags(device.flags);
        msg->set_interface(device.interface);
        msg->set_hwaddress(device.hwAddress);
        msg->set_active_connection(device.active_connection);

        encode(device.ip4config, msg->mutable_ip4config());
        encode(device.ip6config, msg->mutable_ip6config());

        msg->set_ip4connectivity(
            encoded<::cc::platform::netconfig::ConnectivityState>(device.ip4connectivity));
        msg->set_ip6connectivity(
            encoded<::cc::platform::netconfig::ConnectivityState>(device.ip6connectivity));

        if (auto *data = std::get_if<::platform::netconfig::WiredDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wired_data());
        }
        else if (auto *data = std::get_if<::platform::netconfig::WirelessDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wireless_data());
        }
    }

    void decode(const ::cc::platform::netconfig::DeviceData &msg,
                ::platform::netconfig::DeviceData *device) noexcept
    {
        device->type = decoded<NMDeviceType>(msg.type());
        device->state = decoded<NMDeviceState>(msg.state());
        device->state_reason = decoded<NMDeviceStateReason>(msg.state_reason());
        device->flags = msg.flags();
        device->interface = msg.interface();
        device->hwAddress = msg.hwaddress();
        device->active_connection = msg.active_connection();
        decode(msg.ip4config(), &device->ip4config);
        decode(msg.ip6config(), &device->ip6config);
        device->ip4connectivity = decoded<NMConnectivityState>(msg.ip4connectivity());
        device->ip6connectivity = decoded<NMConnectivityState>(msg.ip6connectivity());

        switch (msg.devicedata_case())
        {
        case ::cc::platform::netconfig::DeviceData::DevicedataCase::kWiredData:
            device->specific_data =
                decoded<::platform::netconfig::WiredDeviceData>(msg.wired_data());
            break;

        case ::cc::platform::netconfig::DeviceData::DevicedataCase::kWirelessData:
            device->specific_data =
                decoded<::platform::netconfig::WirelessDeviceData>(msg.wireless_data());
            break;

        default:
            break;
        }
    }

    //==========================================================================
    // ::platform::netconfig::WiredDeviceData

    void encode(const ::platform::netconfig::WiredDeviceData &data,
                ::cc::platform::netconfig::WiredDeviceData *msg) noexcept
    {
        msg->set_speed(data.speed);
    }

    void decode(const ::cc::platform::netconfig::WiredDeviceData &msg,
                ::platform::netconfig::WiredDeviceData *data) noexcept
    {
        data->speed = msg.speed();
    }

    //==========================================================================
    // ::platform::netconfig::WirelessDeviceData

    void encode(const ::platform::netconfig::WirelessDeviceData &data,
                ::cc::platform::netconfig::WirelessDeviceData *msg) noexcept
    {
        msg->set_mode(encoded<::cc::platform::netconfig::WirelessMode>(data.mode));
        msg->set_bitrate(data.bitrate);
        msg->set_active_accesspoint(data.active_accesspoint);
        encode(data.lastScan, msg->mutable_last_scan());
    }

    void decode(const ::cc::platform::netconfig::WirelessDeviceData &msg,
                ::platform::netconfig::WirelessDeviceData *data) noexcept
    {
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->bitrate = msg.bitrate();
        data->active_accesspoint = msg.active_accesspoint();
        decode(msg.last_scan(), &data->lastScan);
    }

    //==========================================================================
    // ::platform::netconfig::DeviceMap

    void encode(const ::platform::netconfig::DeviceMap &map,
                ::cc::platform::netconfig::DeviceMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::DeviceMap &msg,
                ::platform::netconfig::DeviceMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::netconfig::DeviceData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::netconfig::AccessPoint

    void encode(const ::platform::netconfig::AccessPointData &ap,
                ::cc::platform::netconfig::AccessPointData *msg) noexcept
    {
        msg->set_ssid(ap.ssid.data(), ap.ssid.size());
        msg->set_frequency(ap.frequency);
        msg->set_flags(ap.flags);
        msg->set_rsn_flags(ap.rsn_flags);
        msg->set_wpa_flags(ap.wpa_flags);
        msg->set_hwaddress(ap.hwAddress);
        msg->set_mode(encoded<::cc::platform::netconfig::WirelessMode>(ap.mode));
        msg->set_maxbitrate(ap.maxbitrate);
        msg->set_strength(ap.strength);
        encode(ap.lastSeen, msg->mutable_lastseen());
        msg->set_auth_type(encoded<::cc::platform::netconfig::AuthenticationType>(ap.auth_type()));
    }

    void decode(const ::cc::platform::netconfig::AccessPointData &msg,
                ::platform::netconfig::AccessPointData *ap) noexcept
    {
        ap->ssid = msg.ssid();
        ap->frequency = msg.frequency();
        ap->flags = msg.flags();
        ap->rsn_flags = msg.rsn_flags();
        ap->wpa_flags = msg.wpa_flags();
        ap->hwAddress = msg.hwaddress();
        ap->mode = decoded<NM80211Mode>(msg.mode());
        ap->maxbitrate = msg.maxbitrate();
        ap->strength = msg.strength();
        ap->lastSeen = decoded<core::dt::TimePoint>(msg.lastseen());
    }

    //==========================================================================
    // ::platform::netconfig::AccessPointMap

    void encode(const ::platform::netconfig::AccessPointMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::platform::netconfig::AccessPointMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::netconfig::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::netconfig::SSIDMap

    void encode(const ::platform::netconfig::SSIDMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key.to_string()]);
        }
    }

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::platform::netconfig::SSIDMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::netconfig::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::netconfig::WirelessBandSetting

    void encode(::platform::netconfig::WirelessBandSelection band_selection,
                ::cc::platform::netconfig::WirelessBandSelection *protoband) noexcept
    {
        *protoband = static_cast<::cc::platform::netconfig::WirelessBandSelection>(band_selection);
    }

    void decode(::cc::platform::netconfig::WirelessBandSelection proto_band,
                ::platform::netconfig::WirelessBandSelection *band_selection) noexcept
    {
        *band_selection = static_cast<::platform::netconfig::WirelessBandSelection>(proto_band);
    }

    //==========================================================================
    // ::platform::netconfig::NM80211Mode

    void encode(NM80211Mode mode, ::cc::platform::netconfig::WirelessMode *protomode) noexcept
    {
        *protomode = static_cast<::cc::platform::netconfig::WirelessMode>(mode);
    }

    void decode(::cc::platform::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept
    {
        *mode = static_cast<NM80211Mode>(protomode);
    }

    //==========================================================================
    // ::platform::netconfig::KeyManagement

    void encode(::platform::netconfig::KeyManagement key_mgmt,
                ::cc::platform::netconfig::KeyManagement *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::netconfig::KeyManagement>(key_mgmt);
    }

    void decode(::cc::platform::netconfig::KeyManagement proto_key_mgmt,
                ::platform::netconfig::KeyManagement *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::platform::netconfig::KeyManagement>(proto_key_mgmt);
    }

    //==========================================================================
    // ::platform::netconfig::KeyManagement

    void encode(::platform::netconfig::AuthenticationType key_mgmt,
                ::cc::platform::netconfig::AuthenticationType *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::netconfig::AuthenticationType>(key_mgmt);
    }

    void decode(::cc::platform::netconfig::AuthenticationType proto_key_mgmt,
                ::platform::netconfig::AuthenticationType *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::platform::netconfig::AuthenticationType>(proto_key_mgmt);
    }

    //==========================================================================
    // ::platform::netconfig::AuthenticationAlgorithm

    void encode(::platform::netconfig::AuthenticationAlgorithm alg,
                ::cc::platform::netconfig::AuthenticationAlgorithm *proto_alg) noexcept
    {
        *proto_alg = static_cast<::cc::platform::netconfig::AuthenticationAlgorithm>(alg);
    }

    void decode(::cc::platform::netconfig::AuthenticationAlgorithm proto_alg,
                ::platform::netconfig::AuthenticationAlgorithm *alg) noexcept
    {
        *alg = static_cast<::platform::netconfig::AuthenticationAlgorithm>(proto_alg);
    }

    //==========================================================================
    // ::platform::netconfig::EAP_Type

    void encode(::platform::netconfig::EAP_Type eap_type,
                ::cc::platform::netconfig::EAP_Type *proto_eap_type) noexcept
    {
        *proto_eap_type = static_cast<::cc::platform::netconfig::EAP_Type>(eap_type);
    }

    void decode(::cc::platform::netconfig::EAP_Type proto_eap_type,
                ::platform::netconfig::EAP_Type *eap_type) noexcept
    {
        *eap_type = static_cast<::platform::netconfig::EAP_Type>(proto_eap_type);
    }

    //==========================================================================
    // ::platform::netconfig::EAP_Phase2

    void encode(::platform::netconfig::EAP_Phase2 phase2,
                ::cc::platform::netconfig::EAP_Phase2 *proto_phase2) noexcept
    {
        *proto_phase2 = static_cast<::cc::platform::netconfig::EAP_Phase2>(phase2);
    }

    void decode(::cc::platform::netconfig::EAP_Phase2 proto_phase2,
                ::platform::netconfig::EAP_Phase2 *phase2) noexcept
    {
        *phase2 = static_cast<::platform::netconfig::EAP_Phase2>(proto_phase2);
    }

    //==========================================================================
    // ::platform::netconfig::FAST_Provisioning

    void encode(::platform::netconfig::FAST_Provisioning provisioning,
                ::cc::platform::netconfig::FAST_Provisioning *proto_provisioning) noexcept
    {
        *proto_provisioning = static_cast<::cc::platform::netconfig::FAST_Provisioning>(provisioning);
    }

    void decode(::cc::platform::netconfig::FAST_Provisioning proto_provisioning,
                ::platform::netconfig::FAST_Provisioning *provisioning) noexcept
    {
        *provisioning = static_cast<::platform::netconfig::FAST_Provisioning>(proto_provisioning);
    }

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::platform::netconfig::WEP_KeyType *proto_type) noexcept
    {
        *proto_type = static_cast<::cc::platform::netconfig::WEP_KeyType>(type);
    }

    void decode(::cc::platform::netconfig::WEP_KeyType proto_type,
                NMWepKeyType *type) noexcept
    {
        *type = static_cast<NMWepKeyType>(proto_type);
    }

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type, ::cc::platform::netconfig::DeviceType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::netconfig::DeviceType>(type);
    }

    void decode(::cc::platform::netconfig::DeviceType prototype, NMDeviceType *type) noexcept
    {
        *type = static_cast<NMDeviceType>(prototype);
    }

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::platform::netconfig::DeviceState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::netconfig::DeviceState>(state);
    }

    void decode(::cc::platform::netconfig::DeviceState protostate,
                NMDeviceState *state) noexcept
    {
        *state = static_cast<NMDeviceState>(protostate);
    }

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::platform::netconfig::DeviceStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::DeviceStateReason>(reason);
    }

    void decode(::cc::platform::netconfig::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept
    {
        *reason = static_cast<NMDeviceStateReason>(proto);
    }

    //==========================================================================
    // ::platform::netconfig::ConnectionType

    void encode(::platform::netconfig::ConnectionType type,
                ::cc::platform::netconfig::ConnectionType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::netconfig::ConnectionType>(type);
    }

    void decode(::cc::platform::netconfig::ConnectionType prototype,
                ::platform::netconfig::ConnectionType *type) noexcept
    {
        *type = static_cast<::platform::netconfig::ConnectionType>(prototype);
    }

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::platform::netconfig::ActiveConnectionState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::netconfig::ActiveConnectionState>(state);
    }

    void decode(::cc::platform::netconfig::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept
    {
        *state = static_cast<NMActiveConnectionState>(protostate);
    }

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::platform::netconfig::ActiveConnectionStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::ActiveConnectionStateReason>(reason);
    }

    void decode(::cc::platform::netconfig::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept
    {
        *reason = static_cast<NMActiveConnectionStateReason>(proto);
    }

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::platform::netconfig::ConnectivityState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::ConnectivityState>(state);
    }

    void decode(::cc::platform::netconfig::ConnectivityState proto,
                NMConnectivityState *state) noexcept
    {
        *state = static_cast<NMConnectivityState>(proto);
    }

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::platform::netconfig::NetworkState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::NetworkState>(state);
    }

    void decode(::cc::platform::netconfig::NetworkState proto,
                NMState *state) noexcept
    {
        *state = static_cast<NMState>(proto);
    }
}  // namespace protobuf
