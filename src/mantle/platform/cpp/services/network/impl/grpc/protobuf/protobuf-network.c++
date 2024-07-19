/// -*- c++ -*-
//==============================================================================
/// @file protobuf-network.c++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-network.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace core::protobuf
{
    //==========================================================================
    // ::platform::network::GlobalData

    void encode(const ::platform::network::GlobalData &data,
                ::cc::platform::network::GlobalData *msg) noexcept
    {
        msg->set_state(encoded<::cc::platform::network::NetworkState>(data.state));
        msg->set_connectivity(
            encoded<::cc::platform::network::ConnectivityState>(data.connectivity));
        msg->set_wireless_hardware_enabled(data.wireless_hardware_enabled);
        msg->set_wireless_enabled(data.wireless_enabled);
        msg->set_wireless_allowed(data.wireless_allowed);
        msg->set_wireless_band_selection(
            encoded<::cc::platform::network::WirelessBandSelection>(data.wireless_band_selection));
    }

    void decode(const ::cc::platform::network::GlobalData &msg,
                ::platform::network::GlobalData *data) noexcept
    {
        data->state = decoded<NMState>(msg.state());
        data->connectivity = decoded<NMConnectivityState>(msg.connectivity());
        data->wireless_hardware_enabled = msg.wireless_hardware_enabled();
        data->wireless_enabled = msg.wireless_enabled();
        data->wireless_allowed = msg.wireless_allowed();
        data->wireless_band_selection =
            decoded<::platform::network::WirelessBandSelection>(msg.wireless_band_selection());
    }

    //==========================================================================
    // ::platform::network::IPConfig

    void encode(const ::platform::network::IPConfigData &config,
                ::cc::platform::network::IPConfigData *msg) noexcept
    {
        msg->set_method(encoded<::cc::platform::network::IPConfigMethod>(config.method));

        for (const ::platform::network::AddressData &data : config.address_data)
        {
            encode(data, msg->add_address_data());
        }
        msg->set_gateway(config.gateway);
        for (const ::platform::network::IPAddress &dns : config.dns)
        {
            msg->add_dns(dns);
        }
        for (const ::platform::network::Domain &search : config.searches)
        {
            msg->add_searches(search);
        }
    }

    void decode(const ::cc::platform::network::IPConfigData &msg,
                ::platform::network::IPConfigData *config) noexcept
    {
        config->method = decoded<::platform::network::IPConfigMethod>(msg.method());
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
    // ::platform::network::IPConfigMethod

    void encode(::platform::network::IPConfigMethod method,
                ::cc::platform::network::IPConfigMethod *protomethod) noexcept
    {
        *protomethod = static_cast<::cc::platform::network::IPConfigMethod>(method);
    }

    void decode(::cc::platform::network::IPConfigMethod protomethod,
                ::platform::network::IPConfigMethod *method) noexcept
    {
        *method = static_cast<::platform::network::IPConfigMethod>(protomethod);
    }

    //==========================================================================
    // ::platform::network::AddressData

    void encode(const ::platform::network::AddressData &data,
                ::cc::platform::network::AddressData *msg) noexcept
    {
        msg->set_address(data.address);
        msg->set_prefixlength(data.prefixlength);
    }

    void decode(const ::cc::platform::network::AddressData &msg,
                ::platform::network::AddressData *data) noexcept
    {
        data->address = msg.address();
        data->prefixlength = msg.prefixlength();
    }

    //==========================================================================
    // ::platform::network::ConnectionMap

    void encode(const ::platform::network::ConnectionMap &map,
                ::cc::platform::network::ConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::network::ConnectionMap &msg,
                ::platform::network::ConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::network::ConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::network::ConnectionData

    void encode(const ::platform::network::ConnectionData &data,
                ::cc::platform::network::ConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_interface(data.interface);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());

        switch (data.type())
        {
        case ::platform::network::CONN_TYPE_WIRED:
            encode(std::get<::platform::network::WiredConnectionData>(data.specific_data),
                   msg->mutable_wired_data());
            break;

        case ::platform::network::CONN_TYPE_WIRELESS:
            encode(std::get<::platform::network::WirelessConnectionData>(data.specific_data),
                   msg->mutable_wireless_data());
            break;

        default:
            break;
        }
    }

    void decode(const ::cc::platform::network::ConnectionData &msg,
                ::platform::network::ConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->interface = msg.interface();
        data->uuid = msg.uuid();

        decode(msg.ip4config(), &data->ip4config);
        decode(msg.ip6config(), &data->ip6config);

        switch (msg.data_case())
        {
        case ::cc::platform::network::ConnectionData::DataCase::kWiredData:
            data->specific_data =
                decoded<::platform::network::WiredConnectionData>(msg.wired_data());
            break;

        case ::cc::platform::network::ConnectionData::DataCase::kWirelessData:
            data->specific_data =
                decoded<::platform::network::WirelessConnectionData>(msg.wireless_data());
            break;

        default:
            data->specific_data = {};
            break;
        }
    }

    //==========================================================================
    // ::platform::network::WiredConnectionData

    void encode(const ::platform::network::WiredConnectionData &data,
                ::cc::platform::network::WiredConnectionData *msg) noexcept
    {
        msg->set_auto_negotiate(data.auto_negotiate);
    }

    void decode(const ::cc::platform::network::WiredConnectionData &msg,
                ::platform::network::WiredConnectionData *data) noexcept
    {
        data->auto_negotiate = msg.auto_negotiate();
    }

    //==========================================================================
    // ::platform::network::WirelessConnectionData

    void encode(const ::platform::network::WirelessConnectionData &data,
                ::cc::platform::network::WirelessConnectionData *msg) noexcept
    {
        msg->set_ssid(data.ssid.data(), data.ssid.size());
        msg->set_mode(encoded<::cc::platform::network::WirelessMode>(data.mode));
        msg->set_hidden(data.hidden);
        msg->set_tx_power(data.tx_power);
        msg->set_key_mgmt(encoded<::cc::platform::network::KeyManagement>(data.key_mgmt_type()));
        msg->set_auth_protos(data.auth_protos);
        msg->set_auth_type(encoded<::cc::platform::network::AuthenticationType>(data.auth_type()));

        if (auto *wep = std::get_if<::platform::network::AuthenticationData_WEP>(&data.auth))
        {
            encode(*wep, msg->mutable_wep());
        }
        else if (auto *wpa = std::get_if<::platform::network::AuthenticationData_WPA>(&data.auth))
        {
            encode(*wpa, msg->mutable_wpa());
        }
        else if (auto *eap = std::get_if<::platform::network::AuthenticationData_EAP>(&data.auth))
        {
            encode(*eap, msg->mutable_eap());
        }

        if (data.band)
        {
            msg->set_band(encoded<::cc::platform::network::WirelessBandSelection>(*data.band));
        }
    }

    void decode(const ::cc::platform::network::WirelessConnectionData &msg,
                ::platform::network::WirelessConnectionData *data) noexcept
    {
        data->ssid = msg.ssid();
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->hidden = msg.hidden();
        data->tx_power = msg.tx_power();
        data->key_mgmt = decoded<::platform::network::KeyManagement>(msg.key_mgmt());
        data->auth_protos = msg.auth_protos();

        switch (msg.auth_case())
        {
        case ::cc::platform::network::WirelessConnectionData::AuthCase::kWep:
            data->auth = decoded<::platform::network::AuthenticationData_WEP>(msg.wep());
            break;

        case ::cc::platform::network::WirelessConnectionData::AuthCase::kWpa:
            data->auth = decoded<::platform::network::AuthenticationData_WPA>(msg.wpa());
            break;

        case ::cc::platform::network::WirelessConnectionData::AuthCase::kEap:
            data->auth = decoded<::platform::network::AuthenticationData_EAP>(msg.eap());
            break;

        default:
            data->auth = types::nullvalue;
            break;
        }

        if (msg.optional_band_case())
        {
            data->band = decoded<::platform::network::WirelessBandSelection>(msg.band());
        }
    }

    //==========================================================================
    // ::platform::network::ActiveConnectionMap

    void encode(const ::platform::network::ActiveConnectionMap &map,
                ::cc::platform::network::ActiveConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::network::ActiveConnectionMap &msg,
                ::platform::network::ActiveConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::network::ActiveConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::network::ActiveConnectionData

    void encode(const ::platform::network::ActiveConnectionData &data,
                ::cc::platform::network::ActiveConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_type(encoded<::cc::platform::network::ConnectionType>(data.type));
        msg->set_state(encoded<::cc::platform::network::ActiveConnectionState>(data.state));
        msg->set_state_reason(
            encoded<::cc::platform::network::ActiveConnectionStateReason>(data.state_reason));

        msg->set_state_flags(data.state_flags);
        msg->set_default4(data.default4);
        msg->set_default6(data.default6);
        msg->set_vpn(data.vpn);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());
    }

    void decode(const ::cc::platform::network::ActiveConnectionData &msg,
                ::platform::network::ActiveConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->type = decoded<::platform::network::ConnectionType>(msg.type());
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
    // ::platform::network::AuthenticationData_WEP

    void encode(const ::platform::network::AuthenticationData_WEP &auth,
                ::cc::platform::network::AuthenticationData_WEP *msg) noexcept
    {
        msg->set_auth_alg(encoded<::cc::platform::network::AuthenticationAlgorithm>(auth.auth_alg));
        for (const core::types::ByteVector &key : auth.keys)
        {
            msg->add_key(key.data(), key.size());
        }
        msg->set_key_idx(auth.key_idx);
        msg->set_key_type(encoded<::cc::platform::network::WEP_KeyType>(auth.key_type));
    }

    void decode(const ::cc::platform::network::AuthenticationData_WEP &msg,
                ::platform::network::AuthenticationData_WEP *auth) noexcept
    {
        auth->auth_alg =
            decoded<::platform::network::AuthenticationAlgorithm>(msg.auth_alg());

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
    // ::platform::network::AuthenticationData_WPA

    void encode(const ::platform::network::AuthenticationData_WPA &auth,
                ::cc::platform::network::AuthenticationData_WPA *msg) noexcept
    {
        msg->set_psk(auth.psk);
    }

    void decode(const ::cc::platform::network::AuthenticationData_WPA &msg,
                ::platform::network::AuthenticationData_WPA *auth) noexcept
    {
        auth->psk = msg.psk();
    }

    //==========================================================================
    // ::platform::network::AuthenticationData_EAP

    void encode(const ::platform::network::AuthenticationData_EAP &auth,
                ::cc::platform::network::AuthenticationData_EAP *msg) noexcept
    {
        msg->set_auth_alg(
            encoded<::cc::platform::network::AuthenticationAlgorithm>(
                auth.auth_alg));

        msg->set_eap_type(encoded<::cc::platform::network::EAP_Type>(auth.eap_type));
        msg->set_eap_phase2(encoded<::cc::platform::network::EAP_Phase2>(auth.eap_phase2));
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
            encoded<::cc::platform::network::FAST_Provisioning>(auth.fast_provisioning));
    }

    void decode(const ::cc::platform::network::AuthenticationData_EAP &msg,
                ::platform::network::AuthenticationData_EAP *auth) noexcept
    {
        auth->auth_alg =
            decoded<::platform::network::AuthenticationAlgorithm>(msg.auth_alg());

        auth->eap_type = decoded<::platform::network::EAP_Type>(msg.eap_type());
        auth->eap_phase2 = decoded<::platform::network::EAP_Phase2>(msg.eap_phase2());
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
            decoded<::platform::network::FAST_Provisioning>(msg.fast_provisioning());
    }

    //==========================================================================
    // ::platform::network::Device

    void encode(const ::platform::network::DeviceData &device,
                ::cc::platform::network::DeviceData *msg) noexcept
    {
        msg->set_type(encoded<::cc::platform::network::DeviceType>(device.type));
        msg->set_state(encoded<::cc::platform::network::DeviceState>(device.state));
        msg->set_state_reason(
            encoded<::cc::platform::network::DeviceStateReason>(device.state_reason));
        msg->set_flags(device.flags);
        msg->set_interface(device.interface);
        msg->set_hwaddress(device.hwAddress);
        msg->set_active_connection(device.active_connection);

        encode(device.ip4config, msg->mutable_ip4config());
        encode(device.ip6config, msg->mutable_ip6config());

        msg->set_ip4connectivity(
            encoded<::cc::platform::network::ConnectivityState>(device.ip4connectivity));
        msg->set_ip6connectivity(
            encoded<::cc::platform::network::ConnectivityState>(device.ip6connectivity));

        if (auto *data = std::get_if<::platform::network::WiredDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wired_data());
        }
        else if (auto *data = std::get_if<::platform::network::WirelessDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wireless_data());
        }
    }

    void decode(const ::cc::platform::network::DeviceData &msg,
                ::platform::network::DeviceData *device) noexcept
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
        case ::cc::platform::network::DeviceData::DevicedataCase::kWiredData:
            device->specific_data =
                decoded<::platform::network::WiredDeviceData>(msg.wired_data());
            break;

        case ::cc::platform::network::DeviceData::DevicedataCase::kWirelessData:
            device->specific_data =
                decoded<::platform::network::WirelessDeviceData>(msg.wireless_data());
            break;

        default:
            break;
        }
    }

    //==========================================================================
    // ::platform::network::WiredDeviceData

    void encode(const ::platform::network::WiredDeviceData &data,
                ::cc::platform::network::WiredDeviceData *msg) noexcept
    {
        msg->set_speed(data.speed);
    }

    void decode(const ::cc::platform::network::WiredDeviceData &msg,
                ::platform::network::WiredDeviceData *data) noexcept
    {
        data->speed = msg.speed();
    }

    //==========================================================================
    // ::platform::network::WirelessDeviceData

    void encode(const ::platform::network::WirelessDeviceData &data,
                ::cc::platform::network::WirelessDeviceData *msg) noexcept
    {
        msg->set_mode(encoded<::cc::platform::network::WirelessMode>(data.mode));
        msg->set_bitrate(data.bitrate);
        msg->set_active_accesspoint(data.active_accesspoint);
        encode(data.lastScan, msg->mutable_last_scan());
    }

    void decode(const ::cc::platform::network::WirelessDeviceData &msg,
                ::platform::network::WirelessDeviceData *data) noexcept
    {
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->bitrate = msg.bitrate();
        data->active_accesspoint = msg.active_accesspoint();
        decode(msg.last_scan(), &data->lastScan);
    }

    //==========================================================================
    // ::platform::network::DeviceMap

    void encode(const ::platform::network::DeviceMap &map,
                ::cc::platform::network::DeviceMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::network::DeviceMap &msg,
                ::platform::network::DeviceMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::network::DeviceData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::network::AccessPoint

    void encode(const ::platform::network::AccessPointData &ap,
                ::cc::platform::network::AccessPointData *msg) noexcept
    {
        msg->set_ssid(ap.ssid.data(), ap.ssid.size());
        msg->set_frequency(ap.frequency);
        msg->set_flags(ap.flags);
        msg->set_rsn_flags(ap.rsn_flags);
        msg->set_wpa_flags(ap.wpa_flags);
        msg->set_hwaddress(ap.hwAddress);
        msg->set_mode(encoded<::cc::platform::network::WirelessMode>(ap.mode));
        msg->set_maxbitrate(ap.maxbitrate);
        msg->set_strength(ap.strength);
        encode(ap.lastSeen, msg->mutable_lastseen());
        msg->set_auth_type(encoded<::cc::platform::network::AuthenticationType>(ap.auth_type()));
    }

    void decode(const ::cc::platform::network::AccessPointData &msg,
                ::platform::network::AccessPointData *ap) noexcept
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
        ap->lastSeen = decoded<dt::TimePoint>(msg.lastseen());
    }

    //==========================================================================
    // ::platform::network::AccessPointMap

    void encode(const ::platform::network::AccessPointMap &map,
                ::cc::platform::network::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::network::AccessPointMap &msg,
                ::platform::network::AccessPointMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::network::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::network::SSIDMap

    void encode(const ::platform::network::SSIDMap &map,
                ::cc::platform::network::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key.to_string()]);
        }
    }

    void decode(const ::cc::platform::network::AccessPointMap &msg,
                ::platform::network::SSIDMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::platform::network::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::platform::network::WirelessBandSetting

    void encode(::platform::network::WirelessBandSelection band_selection,
                ::cc::platform::network::WirelessBandSelection *protoband) noexcept
    {
        *protoband = static_cast<::cc::platform::network::WirelessBandSelection>(band_selection);
    }

    void decode(::cc::platform::network::WirelessBandSelection proto_band,
                ::platform::network::WirelessBandSelection *band_selection) noexcept
    {
        *band_selection = static_cast<::platform::network::WirelessBandSelection>(proto_band);
    }

    //==========================================================================
    // ::platform::network::NM80211Mode

    void encode(NM80211Mode mode, ::cc::platform::network::WirelessMode *protomode) noexcept
    {
        *protomode = static_cast<::cc::platform::network::WirelessMode>(mode);
    }

    void decode(::cc::platform::network::WirelessMode protomode, NM80211Mode *mode) noexcept
    {
        *mode = static_cast<NM80211Mode>(protomode);
    }

    //==========================================================================
    // ::platform::network::KeyManagement

    void encode(::platform::network::KeyManagement key_mgmt,
                ::cc::platform::network::KeyManagement *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::network::KeyManagement>(key_mgmt);
    }

    void decode(::cc::platform::network::KeyManagement proto_key_mgmt,
                ::platform::network::KeyManagement *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::platform::network::KeyManagement>(proto_key_mgmt);
    }

    //==========================================================================
    // ::platform::network::KeyManagement

    void encode(::platform::network::AuthenticationType key_mgmt,
                ::cc::platform::network::AuthenticationType *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::network::AuthenticationType>(key_mgmt);
    }

    void decode(::cc::platform::network::AuthenticationType proto_key_mgmt,
                ::platform::network::AuthenticationType *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::platform::network::AuthenticationType>(proto_key_mgmt);
    }

    //==========================================================================
    // ::platform::network::AuthenticationAlgorithm

    void encode(::platform::network::AuthenticationAlgorithm alg,
                ::cc::platform::network::AuthenticationAlgorithm *proto_alg) noexcept
    {
        *proto_alg = static_cast<::cc::platform::network::AuthenticationAlgorithm>(alg);
    }

    void decode(::cc::platform::network::AuthenticationAlgorithm proto_alg,
                ::platform::network::AuthenticationAlgorithm *alg) noexcept
    {
        *alg = static_cast<::platform::network::AuthenticationAlgorithm>(proto_alg);
    }

    //==========================================================================
    // ::platform::network::EAP_Type

    void encode(::platform::network::EAP_Type eap_type,
                ::cc::platform::network::EAP_Type *proto_eap_type) noexcept
    {
        *proto_eap_type = static_cast<::cc::platform::network::EAP_Type>(eap_type);
    }

    void decode(::cc::platform::network::EAP_Type proto_eap_type,
                ::platform::network::EAP_Type *eap_type) noexcept
    {
        *eap_type = static_cast<::platform::network::EAP_Type>(proto_eap_type);
    }

    //==========================================================================
    // ::platform::network::EAP_Phase2

    void encode(::platform::network::EAP_Phase2 phase2,
                ::cc::platform::network::EAP_Phase2 *proto_phase2) noexcept
    {
        *proto_phase2 = static_cast<::cc::platform::network::EAP_Phase2>(phase2);
    }

    void decode(::cc::platform::network::EAP_Phase2 proto_phase2,
                ::platform::network::EAP_Phase2 *phase2) noexcept
    {
        *phase2 = static_cast<::platform::network::EAP_Phase2>(proto_phase2);
    }

    //==========================================================================
    // ::platform::network::FAST_Provisioning

    void encode(::platform::network::FAST_Provisioning provisioning,
                ::cc::platform::network::FAST_Provisioning *proto_provisioning) noexcept
    {
        *proto_provisioning = static_cast<::cc::platform::network::FAST_Provisioning>(provisioning);
    }

    void decode(::cc::platform::network::FAST_Provisioning proto_provisioning,
                ::platform::network::FAST_Provisioning *provisioning) noexcept
    {
        *provisioning = static_cast<::platform::network::FAST_Provisioning>(proto_provisioning);
    }

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::platform::network::WEP_KeyType *proto_type) noexcept
    {
        *proto_type = static_cast<::cc::platform::network::WEP_KeyType>(type);
    }

    void decode(::cc::platform::network::WEP_KeyType proto_type,
                NMWepKeyType *type) noexcept
    {
        *type = static_cast<NMWepKeyType>(proto_type);
    }

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type, ::cc::platform::network::DeviceType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::network::DeviceType>(type);
    }

    void decode(::cc::platform::network::DeviceType prototype, NMDeviceType *type) noexcept
    {
        *type = static_cast<NMDeviceType>(prototype);
    }

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::platform::network::DeviceState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::network::DeviceState>(state);
    }

    void decode(::cc::platform::network::DeviceState protostate,
                NMDeviceState *state) noexcept
    {
        *state = static_cast<NMDeviceState>(protostate);
    }

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::platform::network::DeviceStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::network::DeviceStateReason>(reason);
    }

    void decode(::cc::platform::network::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept
    {
        *reason = static_cast<NMDeviceStateReason>(proto);
    }

    //==========================================================================
    // ::platform::network::ConnectionType

    void encode(::platform::network::ConnectionType type,
                ::cc::platform::network::ConnectionType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::network::ConnectionType>(type);
    }

    void decode(::cc::platform::network::ConnectionType prototype,
                ::platform::network::ConnectionType *type) noexcept
    {
        *type = static_cast<::platform::network::ConnectionType>(prototype);
    }

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::platform::network::ActiveConnectionState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::network::ActiveConnectionState>(state);
    }

    void decode(::cc::platform::network::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept
    {
        *state = static_cast<NMActiveConnectionState>(protostate);
    }

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::platform::network::ActiveConnectionStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::network::ActiveConnectionStateReason>(reason);
    }

    void decode(::cc::platform::network::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept
    {
        *reason = static_cast<NMActiveConnectionStateReason>(proto);
    }

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::platform::network::ConnectivityState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::network::ConnectivityState>(state);
    }

    void decode(::cc::platform::network::ConnectivityState proto,
                NMConnectivityState *state) noexcept
    {
        *state = static_cast<NMConnectivityState>(proto);
    }

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::platform::network::NetworkState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::network::NetworkState>(state);
    }

    void decode(::cc::platform::network::NetworkState proto,
                NMState *state) noexcept
    {
        *state = static_cast<NMState>(proto);
    }
}  // core::protobuf
