/// -*- c++ -*-
//==============================================================================
/// @file protobuf-netconfig-types.c++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-netconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // ::netconfig::GlobalData

    void encode(const ::netconfig::GlobalData &data,
                ::cc::platform::netconfig::protobuf::GlobalData *msg) noexcept
    {
        msg->set_state(encoded<::cc::platform::netconfig::protobuf::NetworkState>(data.state));
        msg->set_connectivity(
            encoded<::cc::platform::netconfig::protobuf::ConnectivityState>(data.connectivity));
        msg->set_wireless_hardware_enabled(data.wireless_hardware_enabled);
        msg->set_wireless_enabled(data.wireless_enabled);
        msg->set_wireless_allowed(data.wireless_allowed);
        msg->set_wireless_band_selection(
            encoded<::cc::platform::netconfig::protobuf::WirelessBandSelection>(data.wireless_band_selection));
    }

    void decode(const ::cc::platform::netconfig::protobuf::GlobalData &msg,
                ::netconfig::GlobalData *data) noexcept
    {
        data->state = decoded<NMState>(msg.state());
        data->connectivity = decoded<NMConnectivityState>(msg.connectivity());
        data->wireless_hardware_enabled = msg.wireless_hardware_enabled();
        data->wireless_enabled = msg.wireless_enabled();
        data->wireless_allowed = msg.wireless_allowed();
        data->wireless_band_selection =
            decoded<::netconfig::WirelessBandSelection>(msg.wireless_band_selection());
    }

    //==========================================================================
    // ::netconfig::IPConfig

    void encode(const ::netconfig::IPConfigData &config,
                ::cc::platform::netconfig::protobuf::IPConfigData *msg) noexcept
    {
        msg->set_method(encoded<::cc::platform::netconfig::protobuf::IPConfigMethod>(config.method));

        for (const ::netconfig::AddressData &data : config.address_data)
        {
            encode(data, msg->add_address_data());
        }
        msg->set_gateway(config.gateway);
        for (const ::netconfig::IPAddress &dns : config.dns)
        {
            msg->add_dns(dns);
        }
        for (const ::netconfig::Domain &search : config.searches)
        {
            msg->add_searches(search);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::IPConfigData &msg,
                ::netconfig::IPConfigData *config) noexcept
    {
        config->method = decoded<::netconfig::IPConfigMethod>(msg.method());
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
    // ::netconfig::IPConfigMethod

    void encode(::netconfig::IPConfigMethod method,
                ::cc::platform::netconfig::protobuf::IPConfigMethod *protomethod) noexcept
    {
        *protomethod = static_cast<::cc::platform::netconfig::protobuf::IPConfigMethod>(method);
    }

    void decode(::cc::platform::netconfig::protobuf::IPConfigMethod protomethod,
                ::netconfig::IPConfigMethod *method) noexcept
    {
        *method = static_cast<::netconfig::IPConfigMethod>(protomethod);
    }

    //==========================================================================
    // ::netconfig::AddressData

    void encode(const ::netconfig::AddressData &data,
                ::cc::platform::netconfig::protobuf::AddressData *msg) noexcept
    {
        msg->set_address(data.address);
        msg->set_prefixlength(data.prefixlength);
    }

    void decode(const ::cc::platform::netconfig::protobuf::AddressData &msg,
                ::netconfig::AddressData *data) noexcept
    {
        data->address = msg.address();
        data->prefixlength = msg.prefixlength();
    }

    //==========================================================================
    // ::netconfig::ConnectionMap

    void encode(const ::netconfig::ConnectionMap &map,
                ::cc::platform::netconfig::protobuf::ConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::ConnectionMap &msg,
                ::netconfig::ConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::netconfig::ConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::netconfig::ConnectionData

    void encode(const ::netconfig::ConnectionData &data,
                ::cc::platform::netconfig::protobuf::ConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_interface(data.interface);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());

        switch (data.type())
        {
        case ::netconfig::CONN_TYPE_WIRED:
            encode(std::get<::netconfig::WiredConnectionData>(data.specific_data),
                   msg->mutable_wired_data());
            break;

        case ::netconfig::CONN_TYPE_WIRELESS:
            encode(std::get<::netconfig::WirelessConnectionData>(data.specific_data),
                   msg->mutable_wireless_data());
            break;

        default:
            break;
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::ConnectionData &msg,
                ::netconfig::ConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->interface = msg.interface();
        data->uuid = msg.uuid();

        decode(msg.ip4config(), &data->ip4config);
        decode(msg.ip6config(), &data->ip6config);

        switch (msg.data_case())
        {
        case ::cc::platform::netconfig::protobuf::ConnectionData::DataCase::kWiredData:
            data->specific_data =
                decoded<::netconfig::WiredConnectionData>(msg.wired_data());
            break;

        case ::cc::platform::netconfig::protobuf::ConnectionData::DataCase::kWirelessData:
            data->specific_data =
                decoded<::netconfig::WirelessConnectionData>(msg.wireless_data());
            break;

        default:
            data->specific_data = {};
            break;
        }
    }

    //==========================================================================
    // ::netconfig::WiredConnectionData

    void encode(const ::netconfig::WiredConnectionData &data,
                ::cc::platform::netconfig::protobuf::WiredConnectionData *msg) noexcept
    {
        msg->set_auto_negotiate(data.auto_negotiate);
    }

    void decode(const ::cc::platform::netconfig::protobuf::WiredConnectionData &msg,
                ::netconfig::WiredConnectionData *data) noexcept
    {
        data->auto_negotiate = msg.auto_negotiate();
    }

    //==========================================================================
    // ::netconfig::WirelessConnectionData

    void encode(const ::netconfig::WirelessConnectionData &data,
                ::cc::platform::netconfig::protobuf::WirelessConnectionData *msg) noexcept
    {
        msg->set_ssid(data.ssid.data(), data.ssid.size());
        msg->set_mode(encoded<::cc::platform::netconfig::protobuf::WirelessMode>(data.mode));
        msg->set_hidden(data.hidden);
        msg->set_tx_power(data.tx_power);
        msg->set_key_mgmt(encoded<::cc::platform::netconfig::protobuf::KeyManagement>(data.key_mgmt_type()));
        msg->set_auth_protos(data.auth_protos);
        msg->set_auth_type(encoded<::cc::platform::netconfig::protobuf::AuthenticationType>(data.auth_type()));

        if (auto *wep = std::get_if<::netconfig::WEP_Data>(&data.auth))
        {
            encode(*wep, msg->mutable_wep());
        }
        else if (auto *wpa = std::get_if<::netconfig::WPA_Data>(&data.auth))
        {
            encode(*wpa, msg->mutable_wpa());
        }
        else if (auto *eap = std::get_if<::netconfig::EAP_Data>(&data.auth))
        {
            encode(*eap, msg->mutable_eap());
        }

        if (data.band)
        {
            msg->set_band(encoded<::cc::platform::netconfig::protobuf::WirelessBandSelection>(*data.band));
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::WirelessConnectionData &msg,
                ::netconfig::WirelessConnectionData *data) noexcept
    {
        data->ssid = msg.ssid();
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->hidden = msg.hidden();
        data->tx_power = msg.tx_power();
        data->key_mgmt = decoded<::netconfig::KeyManagement>(msg.key_mgmt());
        data->auth_protos = msg.auth_protos();

        switch (msg.auth_case())
        {
        case ::cc::platform::netconfig::protobuf::WirelessConnectionData::AuthCase::kWep:
            data->auth = decoded<::netconfig::WEP_Data>(msg.wep());
            break;

        case ::cc::platform::netconfig::protobuf::WirelessConnectionData::AuthCase::kWpa:
            data->auth = decoded<::netconfig::WPA_Data>(msg.wpa());
            break;

        case ::cc::platform::netconfig::protobuf::WirelessConnectionData::AuthCase::kEap:
            data->auth = decoded<::netconfig::EAP_Data>(msg.eap());
            break;

        default:
            data->auth = {};
            break;
        }

        if (msg.has_band())
        {
            data->band = decoded<::netconfig::WirelessBandSelection>(msg.band());
        }
    }

    //==========================================================================
    // ::netconfig::ActiveConnectionMap

    void encode(const ::netconfig::ActiveConnectionMap &map,
                ::cc::platform::netconfig::protobuf::ActiveConnectionMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::ActiveConnectionMap &msg,
                ::netconfig::ActiveConnectionMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::netconfig::ActiveConnectionData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::netconfig::ActiveConnectionData

    void encode(const ::netconfig::ActiveConnectionData &data,
                ::cc::platform::netconfig::protobuf::ActiveConnectionData *msg) noexcept
    {
        msg->set_id(data.id);
        msg->set_type(encoded<::cc::platform::netconfig::protobuf::ConnectionType>(data.type));
        msg->set_state(encoded<::cc::platform::netconfig::protobuf::ActiveConnectionState>(data.state));
        msg->set_state_reason(
            encoded<::cc::platform::netconfig::protobuf::ActiveConnectionStateReason>(data.state_reason));

        msg->set_state_flags(data.state_flags);
        msg->set_default4(data.default4);
        msg->set_default6(data.default6);
        msg->set_vpn(data.vpn);
        msg->set_uuid(data.uuid);

        encode(data.ip4config, msg->mutable_ip4config());
        encode(data.ip6config, msg->mutable_ip6config());
    }

    void decode(const ::cc::platform::netconfig::protobuf::ActiveConnectionData &msg,
                ::netconfig::ActiveConnectionData *data) noexcept
    {
        data->id = msg.id();
        data->type = decoded<::netconfig::ConnectionType>(msg.type());
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
    // ::netconfig::WEP_Data

    void encode(const ::netconfig::WEP_Data &auth,
                ::cc::platform::netconfig::protobuf::WEPData *msg) noexcept
    {
        msg->set_auth_alg(encoded<::cc::platform::netconfig::protobuf::AuthenticationAlgorithm>(auth.auth_alg));
        for (const core::types::ByteVector &key : auth.keys)
        {
            msg->add_key(key.data(), key.size());
        }
        msg->set_key_idx(auth.key_idx);
        msg->set_key_type(encoded<::cc::platform::netconfig::protobuf::WEPKeyType>(auth.key_type));
    }

    void decode(const ::cc::platform::netconfig::protobuf::WEPData &msg,
                ::netconfig::WEP_Data *auth) noexcept
    {
        auth->auth_alg =
            decoded<::netconfig::AuthenticationAlgorithm>(msg.auth_alg());

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
    // ::netconfig::WPA_Data

    void encode(const ::netconfig::WPA_Data &auth,
                ::cc::platform::netconfig::protobuf::WPAData *msg) noexcept
    {
        msg->set_psk(auth.psk);
    }

    void decode(const ::cc::platform::netconfig::protobuf::WPAData &msg,
                ::netconfig::WPA_Data *auth) noexcept
    {
        auth->psk = msg.psk();
    }

    //==========================================================================
    // ::netconfig::EAP_Data

    void encode(const ::netconfig::EAP_Data &auth,
                ::cc::platform::netconfig::protobuf::EAPData *msg) noexcept
    {
        msg->set_auth_alg(
            encoded<::cc::platform::netconfig::protobuf::AuthenticationAlgorithm>(
                auth.auth_alg));

        msg->set_eap_type(encoded<::cc::platform::netconfig::protobuf::EAPType>(auth.eap_type));
        msg->set_eap_phase2(encoded<::cc::platform::netconfig::protobuf::EAPPhase2>(auth.eap_phase2));
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
            encoded<::cc::platform::netconfig::protobuf::FASTProvisioning>(auth.fast_provisioning));
    }

    void decode(const ::cc::platform::netconfig::protobuf::EAPData &msg,
                ::netconfig::EAP_Data *auth) noexcept
    {
        auth->auth_alg =
            decoded<::netconfig::AuthenticationAlgorithm>(msg.auth_alg());

        auth->eap_type = decoded<::netconfig::EAP_Type>(msg.eap_type());
        auth->eap_phase2 = decoded<::netconfig::EAP_Phase2>(msg.eap_phase2());
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
            decoded<::netconfig::FAST_Provisioning>(msg.fast_provisioning());
    }

    //==========================================================================
    // ::netconfig::Device

    void encode(const ::netconfig::DeviceData &device,
                ::cc::platform::netconfig::protobuf::DeviceData *msg) noexcept
    {
        msg->set_type(encoded<::cc::platform::netconfig::protobuf::DeviceType>(device.type));
        msg->set_state(encoded<::cc::platform::netconfig::protobuf::DeviceState>(device.state));
        msg->set_state_reason(
            encoded<::cc::platform::netconfig::protobuf::DeviceStateReason>(device.state_reason));
        msg->set_flags(device.flags);
        msg->set_interface(device.interface);
        msg->set_hwaddress(device.hwAddress);
        msg->set_active_connection(device.active_connection);

        encode(device.ip4config, msg->mutable_ip4config());
        encode(device.ip6config, msg->mutable_ip6config());

        msg->set_ip4connectivity(
            encoded<::cc::platform::netconfig::protobuf::ConnectivityState>(device.ip4connectivity));
        msg->set_ip6connectivity(
            encoded<::cc::platform::netconfig::protobuf::ConnectivityState>(device.ip6connectivity));

        if (auto *data = std::get_if<::netconfig::WiredDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wired_data());
        }
        else if (auto *data = std::get_if<::netconfig::WirelessDeviceData>(&device.specific_data))
        {
            encode(*data, msg->mutable_wireless_data());
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::DeviceData &msg,
                ::netconfig::DeviceData *device) noexcept
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
        case ::cc::platform::netconfig::protobuf::DeviceData::DevicedataCase::kWiredData:
            device->specific_data =
                decoded<::netconfig::WiredDeviceData>(msg.wired_data());
            break;

        case ::cc::platform::netconfig::protobuf::DeviceData::DevicedataCase::kWirelessData:
            device->specific_data =
                decoded<::netconfig::WirelessDeviceData>(msg.wireless_data());
            break;

        default:
            break;
        }
    }

    //==========================================================================
    // ::netconfig::WiredDeviceData

    void encode(const ::netconfig::WiredDeviceData &data,
                ::cc::platform::netconfig::protobuf::WiredDeviceData *msg) noexcept
    {
        msg->set_speed(data.speed);
    }

    void decode(const ::cc::platform::netconfig::protobuf::WiredDeviceData &msg,
                ::netconfig::WiredDeviceData *data) noexcept
    {
        data->speed = msg.speed();
    }

    //==========================================================================
    // ::netconfig::WirelessDeviceData

    void encode(const ::netconfig::WirelessDeviceData &data,
                ::cc::platform::netconfig::protobuf::WirelessDeviceData *msg) noexcept
    {
        msg->set_mode(encoded<::cc::platform::netconfig::protobuf::WirelessMode>(data.mode));
        msg->set_bitrate(data.bitrate);
        msg->set_active_accesspoint(data.active_accesspoint);
        encode(data.lastScan, msg->mutable_last_scan());
    }

    void decode(const ::cc::platform::netconfig::protobuf::WirelessDeviceData &msg,
                ::netconfig::WirelessDeviceData *data) noexcept
    {
        data->mode = decoded<NM80211Mode>(msg.mode());
        data->bitrate = msg.bitrate();
        data->active_accesspoint = msg.active_accesspoint();
        decode(msg.last_scan(), &data->lastScan);
    }

    //==========================================================================
    // ::netconfig::DeviceMap

    void encode(const ::netconfig::DeviceMap &map,
                ::cc::platform::netconfig::protobuf::DeviceMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::DeviceMap &msg,
                ::netconfig::DeviceMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::netconfig::DeviceData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::netconfig::AccessPoint

    void encode(const ::netconfig::AccessPointData &ap,
                ::cc::platform::netconfig::protobuf::AccessPointData *msg) noexcept
    {
        msg->set_ssid(ap.ssid.data(), ap.ssid.size());
        msg->set_frequency(ap.frequency);
        msg->set_flags(ap.flags);
        msg->set_rsn_flags(ap.rsn_flags);
        msg->set_wpa_flags(ap.wpa_flags);
        msg->set_hwaddress(ap.hwAddress);
        msg->set_mode(encoded<::cc::platform::netconfig::protobuf::WirelessMode>(ap.mode));
        msg->set_maxbitrate(ap.maxbitrate);
        msg->set_strength(ap.strength);
        encode(ap.lastSeen, msg->mutable_lastseen());
        msg->set_auth_type(encoded<::cc::platform::netconfig::protobuf::AuthenticationType>(ap.auth_type()));
    }

    void decode(const ::cc::platform::netconfig::protobuf::AccessPointData &msg,
                ::netconfig::AccessPointData *ap) noexcept
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
    // ::netconfig::AccessPointMap

    void encode(const ::netconfig::AccessPointMap &map,
                ::cc::platform::netconfig::protobuf::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key]);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::AccessPointMap &msg,
                ::netconfig::AccessPointMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::netconfig::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::netconfig::SSIDMap

    void encode(const ::netconfig::SSIDMap &map,
                ::cc::platform::netconfig::protobuf::AccessPointMap *msg) noexcept
    {
        auto *encoded = msg->mutable_map();
        for (const auto &[key, ref] : map)
        {
            encode(*ref, &(*encoded)[key.to_string()]);
        }
    }

    void decode(const ::cc::platform::netconfig::protobuf::AccessPointMap &msg,
                ::netconfig::SSIDMap *map) noexcept
    {
        for (const auto &[key, data] : msg.map())
        {
            auto ref = (*map)[key] = std::make_shared<::netconfig::AccessPointData>();
            decode(data, ref.get());
        }
    }

    //==========================================================================
    // ::netconfig::WirelessBandSetting

    void encode(::netconfig::WirelessBandSelection band_selection,
                ::cc::platform::netconfig::protobuf::WirelessBandSelection *protoband) noexcept
    {
        *protoband = static_cast<::cc::platform::netconfig::protobuf::WirelessBandSelection>(band_selection);
    }

    void decode(::cc::platform::netconfig::protobuf::WirelessBandSelection proto_band,
                ::netconfig::WirelessBandSelection *band_selection) noexcept
    {
        *band_selection = static_cast<::netconfig::WirelessBandSelection>(proto_band);
    }

    //==========================================================================
    // ::netconfig::NM80211Mode

    void encode(NM80211Mode mode, ::cc::platform::netconfig::protobuf::WirelessMode *protomode) noexcept
    {
        *protomode = static_cast<::cc::platform::netconfig::protobuf::WirelessMode>(mode);
    }

    void decode(::cc::platform::netconfig::protobuf::WirelessMode protomode, NM80211Mode *mode) noexcept
    {
        *mode = static_cast<NM80211Mode>(protomode);
    }

    //==========================================================================
    // ::netconfig::KeyManagement

    void encode(::netconfig::KeyManagement key_mgmt,
                ::cc::platform::netconfig::protobuf::KeyManagement *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::netconfig::protobuf::KeyManagement>(key_mgmt);
    }

    void decode(::cc::platform::netconfig::protobuf::KeyManagement proto_key_mgmt,
                ::netconfig::KeyManagement *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::netconfig::KeyManagement>(proto_key_mgmt);
    }

    //==========================================================================
    // ::netconfig::KeyManagement

    void encode(::netconfig::AuthenticationType key_mgmt,
                ::cc::platform::netconfig::protobuf::AuthenticationType *proto_key_mgmt) noexcept
    {
        *proto_key_mgmt = static_cast<::cc::platform::netconfig::protobuf::AuthenticationType>(key_mgmt);
    }

    void decode(::cc::platform::netconfig::protobuf::AuthenticationType proto_key_mgmt,
                ::netconfig::AuthenticationType *key_mgmt) noexcept
    {
        *key_mgmt = static_cast<::netconfig::AuthenticationType>(proto_key_mgmt);
    }

    //==========================================================================
    // ::netconfig::AuthenticationAlgorithm

    void encode(::netconfig::AuthenticationAlgorithm alg,
                ::cc::platform::netconfig::protobuf::AuthenticationAlgorithm *proto_alg) noexcept
    {
        *proto_alg = static_cast<::cc::platform::netconfig::protobuf::AuthenticationAlgorithm>(alg);
    }

    void decode(::cc::platform::netconfig::protobuf::AuthenticationAlgorithm proto_alg,
                ::netconfig::AuthenticationAlgorithm *alg) noexcept
    {
        *alg = static_cast<::netconfig::AuthenticationAlgorithm>(proto_alg);
    }

    //==========================================================================
    // ::netconfig::EAP_Type

    void encode(::netconfig::EAP_Type eap_type,
                ::cc::platform::netconfig::protobuf::EAPType *proto_eap_type) noexcept
    {
        *proto_eap_type = static_cast<::cc::platform::netconfig::protobuf::EAPType>(eap_type);
    }

    void decode(::cc::platform::netconfig::protobuf::EAPType proto_eap_type,
                ::netconfig::EAP_Type *eap_type) noexcept
    {
        *eap_type = static_cast<::netconfig::EAP_Type>(proto_eap_type);
    }

    //==========================================================================
    // ::netconfig::EAP_Phase2

    void encode(::netconfig::EAP_Phase2 phase2,
                ::cc::platform::netconfig::protobuf::EAPPhase2 *proto_phase2) noexcept
    {
        *proto_phase2 = static_cast<::cc::platform::netconfig::protobuf::EAPPhase2>(phase2);
    }

    void decode(::cc::platform::netconfig::protobuf::EAPPhase2 proto_phase2,
                ::netconfig::EAP_Phase2 *phase2) noexcept
    {
        *phase2 = static_cast<::netconfig::EAP_Phase2>(proto_phase2);
    }

    //==========================================================================
    // ::netconfig::FAST_Provisioning

    void encode(::netconfig::FAST_Provisioning provisioning,
                ::cc::platform::netconfig::protobuf::FASTProvisioning *proto_provisioning) noexcept
    {
        *proto_provisioning = static_cast<::cc::platform::netconfig::protobuf::FASTProvisioning>(provisioning);
    }

    void decode(::cc::platform::netconfig::protobuf::FASTProvisioning proto_provisioning,
                ::netconfig::FAST_Provisioning *provisioning) noexcept
    {
        *provisioning = static_cast<::netconfig::FAST_Provisioning>(proto_provisioning);
    }

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::platform::netconfig::protobuf::WEPKeyType *proto_type) noexcept
    {
        *proto_type = static_cast<::cc::platform::netconfig::protobuf::WEPKeyType>(type);
    }

    void decode(::cc::platform::netconfig::protobuf::WEPKeyType proto_type,
                NMWepKeyType *type) noexcept
    {
        *type = static_cast<NMWepKeyType>(proto_type);
    }

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type, ::cc::platform::netconfig::protobuf::DeviceType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::netconfig::protobuf::DeviceType>(type);
    }

    void decode(::cc::platform::netconfig::protobuf::DeviceType prototype, NMDeviceType *type) noexcept
    {
        *type = static_cast<NMDeviceType>(prototype);
    }

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::platform::netconfig::protobuf::DeviceState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::netconfig::protobuf::DeviceState>(state);
    }

    void decode(::cc::platform::netconfig::protobuf::DeviceState protostate,
                NMDeviceState *state) noexcept
    {
        *state = static_cast<NMDeviceState>(protostate);
    }

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::platform::netconfig::protobuf::DeviceStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::protobuf::DeviceStateReason>(reason);
    }

    void decode(::cc::platform::netconfig::protobuf::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept
    {
        *reason = static_cast<NMDeviceStateReason>(proto);
    }

    //==========================================================================
    // ::netconfig::ConnectionType

    void encode(::netconfig::ConnectionType type,
                ::cc::platform::netconfig::protobuf::ConnectionType *prototype) noexcept
    {
        *prototype = static_cast<::cc::platform::netconfig::protobuf::ConnectionType>(type);
    }

    void decode(::cc::platform::netconfig::protobuf::ConnectionType prototype,
                ::netconfig::ConnectionType *type) noexcept
    {
        *type = static_cast<::netconfig::ConnectionType>(prototype);
    }

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::platform::netconfig::protobuf::ActiveConnectionState *protostate) noexcept
    {
        *protostate = static_cast<::cc::platform::netconfig::protobuf::ActiveConnectionState>(state);
    }

    void decode(::cc::platform::netconfig::protobuf::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept
    {
        *state = static_cast<NMActiveConnectionState>(protostate);
    }

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::platform::netconfig::protobuf::ActiveConnectionStateReason *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::protobuf::ActiveConnectionStateReason>(reason);
    }

    void decode(::cc::platform::netconfig::protobuf::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept
    {
        *reason = static_cast<NMActiveConnectionStateReason>(proto);
    }

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::platform::netconfig::protobuf::ConnectivityState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::protobuf::ConnectivityState>(state);
    }

    void decode(::cc::platform::netconfig::protobuf::ConnectivityState proto,
                NMConnectivityState *state) noexcept
    {
        *state = static_cast<NMConnectivityState>(proto);
    }

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::platform::netconfig::protobuf::NetworkState *proto) noexcept
    {
        *proto = static_cast<::cc::platform::netconfig::protobuf::NetworkState>(state);
    }

    void decode(::cc::platform::netconfig::protobuf::NetworkState proto,
                NMState *state) noexcept
    {
        *state = static_cast<NMState>(proto);
    }
}  // namespace protobuf
