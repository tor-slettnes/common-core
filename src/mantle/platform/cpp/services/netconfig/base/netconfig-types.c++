// -*- c++ -*-
//==============================================================================
/// @file netconfig-types.c++
/// @brief NetConfig service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "netconfig-types.h++"

namespace netconfig
{
    void SystemData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->hostname.size(), "hostname", this->hostname);
    }

    void AddressData::to_stream(std::ostream &stream) const
    {
        if (!this->address.empty())
        {
            stream << this->address << "/" << this->prefixlength;
        }
    }

    void AddressData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"address", this->address},
            {"prefix_length", this->prefixlength},
        });
    }

    void IPConfigData::clear()
    {
        this->method = METHOD_AUTO;
        this->address_data.clear();
        this->gateway.clear();
        this->dns.clear();
        this->searches.clear();
    }

    void IPConfigData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->method,
                          "method",
                          core::str::convert_from(this->method));

        tvlist->append_if(this->address_data.size(),
                          "address_data",
                          core::str::convert_from(this->address_data));

        tvlist->append_if(this->gateway.size(),
                          "gateway",
                          this->gateway);

        tvlist->append_if(this->dns.size(),
                          "dns",
                          core::str::convert_from(this->dns));

        tvlist->append("searches",
                       this->searches.size());
    }

    void WiredConnectionData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("auto_negotiate", this->auto_negotiate);
    }

    void WEP_Data::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->auth_alg,
                          "auth_alg",
                          this->auth_alg);

        tvlist->append("key_idx", this->key_idx);
        for (int i = 0; i < this->keys.size(); i++)
        {
            tvlist->append_if(this->keys.at(i).size(),
                              core::str::format("key%d", i),
                              core::str::obfuscated(this->keys.at(i).stringview()));
        }
        tvlist->append_if(this->key_type, "key_type", this->key_type);
    }

    void WPA_Data::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("psk", core::str::obfuscated(this->psk));
    }

    void EAP_Data::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->auth_alg,
                          "auth_alg",
                          core::str::convert_from(this->auth_alg));
        tvlist->append_if(this->eap_type,
                          "eap_type",
                          core::str::convert_from(this->eap_type));
        tvlist->append_if(this->eap_phase2,
                          "eap_phase2",
                          core::str::convert_from(this->eap_phase2));
        tvlist->append_if(!this->anonymous_identity.empty(),
                          "anonymous_identity",
                          this->anonymous_identity);
        tvlist->append_if(!this->domain.empty(),
                          "domain",
                          this->domain);
        tvlist->append_if(!this->identity.empty(),
                          "identity",
                          this->identity);
        tvlist->append_if(!this->password.empty(),
                          "password",
                          core::str::obfuscated(this->password));
        tvlist->append_if(!this->ca_cert.empty(),
                          "ca_cert",
                          this->ca_cert);
        tvlist->append_if(!this->client_cert.empty(),
                          "client_cert",
                          this->client_cert);
        tvlist->append_if(!this->client_cert_key.empty(),
                          "client_cert_key",
                          this->client_cert_key);
        tvlist->append_if(!this->client_cert_password.empty(),
                          "client_cert_password",
                          core::str::obfuscated(this->client_cert_password));
        tvlist->append_if(!this->pac_file.empty(),
                          "pac_file",
                          this->pac_file);
        tvlist->append_if(this->fast_provisioning,
                          "fast_provisioning",
                          core::str::convert_from(this->fast_provisioning));
    }

    KeyManagement WirelessConnectionData::key_mgmt_type() const
    {
        if (this->key_mgmt == KEY_EMPTY)
        {
            switch (this->auth_type())
            {
            case AUTH_TYPE_WEP:
                return KEY_WEP;

            case AUTH_TYPE_WPA:
                return KEY_PSK;

            case AUTH_TYPE_EAP:
                return KEY_EAP;

            default:
                return KEY_EMPTY;
            }
        }
        return this->key_mgmt;
    }

    AuthenticationType WirelessConnectionData::auth_type() const
    {
        return static_cast<AuthenticationType>(auth.index());
    }

    WEP_Data *WirelessConnectionData::auth_wep()
    {
        return std::get_if<WEP_Data>(&this->auth);
    }

    WPA_Data *WirelessConnectionData::auth_wpa()
    {
        return std::get_if<WPA_Data>(&this->auth);
    }

    EAP_Data *WirelessConnectionData::auth_eap()
    {
        return std::get_if<EAP_Data>(&this->auth);
    }

    void WirelessConnectionData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(!this->ssid.empty(), "ssid", this->ssid.as_string());
        tvlist->append_if(this->mode, "ap_mode", core::str::convert_from(this->mode));
        tvlist->append("hidden", this->hidden);
        tvlist->append("tx_power", this->tx_power);
        tvlist->append_if(this->band.has_value(), "band", core::str::convert_from(this->band));
        tvlist->append_if(this->key_mgmt, "key_mgmt", core::str::convert_from(this->key_mgmt));
        tvlist->append_if(this->auth_protos, "auth_protos", this->auth_protos);

        switch (this->auth_type())
        {
        case AUTH_TYPE_WEP:
            tvlist->append("wep", std::get<WEP_Data>(this->auth).as_tvlist());
            break;

        case AUTH_TYPE_WPA:
            tvlist->append("wpa", std::get<WPA_Data>(this->auth).as_tvlist());
            break;

        case AUTH_TYPE_EAP:
            tvlist->append("eap", std::get<EAP_Data>(this->auth).as_tvlist());
            break;

        default:
            break;
        }
    }

    std::string ConnectionData::key() const
    {
        return this->id;
    }

    ConnectionType ConnectionData::type() const
    {
        return static_cast<ConnectionType>(specific_data.index());
    }

    WiredConnectionData *ConnectionData::wired_data()
    {
        return std::get_if<WiredConnectionData>(&this->specific_data);
    }

    WirelessConnectionData *ConnectionData::wifi_data()
    {
        return std::get_if<WirelessConnectionData>(&this->specific_data);
    }

    bool ConnectionData::is_valid() const
    {
        return (this->type() != CONN_TYPE_UNKNOWN) &&
               (this->ip4config.method != METHOD_NONE) &&
               (this->ip6config.method != METHOD_NONE);
    }

    void ConnectionData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(!this->id.empty(), "id", this->id);
        tvlist->append_if(!this->uuid.empty(), "uuid", this->uuid);
        tvlist->append_if(!this->interface.empty(), "interface", this->interface);

        switch (this->type())
        {
        case CONN_TYPE_WIRED:
            tvlist->append(
                "wired",
                std::get<WiredConnectionData>(this->specific_data).as_tvlist());
            break;

        case CONN_TYPE_WIRELESS:
            tvlist->append(
                "wireless",
                std::get<WirelessConnectionData>(this->specific_data).as_tvlist());
            break;

        default:
            break;
        }

        tvlist->append("ip4config", this->ip4config.as_tvlist());
        tvlist->append("ip6config", this->ip6config.as_tvlist());
    }

    std::string ActiveConnectionData::key() const
    {
        return this->id;
    }

    void ActiveConnectionData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->extend({
            {"id", this->id},
            {"uuid", this->uuid},
            {"type", core::str::convert_from(this->type)},
            {"state", core::str::convert_from(this->state)},
            {"reason", core::str::convert_from(this->state_reason)},
            {"flags", this->state_flags},
            {"default4", this->default4},
            {"ip4config", this->ip4config.as_tvlist()},
            {"default6", this->default6},
            {"ip6config", this->ip6config.as_tvlist()},
            {"vpn", this->vpn},
        });
    }

    bool ActiveConnectionData::is_connected() const
    {
        return (this->state == NM_ACTIVE_CONNECTION_STATE_ACTIVATED);
    }

    bool ActiveConnectionData::is_busy() const
    {
        switch (this->state)
        {
        case NM_ACTIVE_CONNECTION_STATE_ACTIVATING:
        case NM_ACTIVE_CONNECTION_STATE_DEACTIVATING:
            return true;

        default:
            return false;
        }
    }

    bool ActiveConnectionData::has_gateway() const
    {
        return ((this->default4 && this->ip4config.gateway.size()) ||
                (this->default6 && this->ip6config.gateway.size()));
    }

    std::string AccessPointData::key() const
    {
        return this->hwAddress;
    }

    uint AccessPointData::auth_flags() const
    {
        return this->rsn_flags ? this->rsn_flags : this->wpa_flags;
    }

    AuthenticationType AccessPointData::auth_type() const
    {
        if (uint auth = this->auth_flags())
        {
            if (auth & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
            {
                return AUTH_TYPE_EAP;
            }
            else if (auth & (NM_802_11_AP_SEC_KEY_MGMT_PSK | NM_802_11_AP_SEC_KEY_MGMT_SAE))
            {
                return AUTH_TYPE_WPA;
            }
            else
            {
                return AUTH_TYPE_UNKNOWN;
            }
        }
        else if (this->flags & NM_802_11_AP_FLAGS_PRIVACY)
        {
            return AUTH_TYPE_WEP;
        }
        else
        {
            return AUTH_TYPE_NONE;
        }
    }

    bool AccessPointData::auth_required() const
    {
        return this->auth_type() != AUTH_TYPE_NONE;
    }

    FrequencyRangeMap AccessPointData::frequency_ranges =
        {
            {BAND_A, {5000, 6000}},
            {BAND_BG, {2400, 2500}},
    };

    WirelessBandSelection AccessPointData::band() const
    {
        for (const auto &[band, range] : AccessPointData::frequency_ranges)
        {
            if ((this->frequency >= range.first) && (this->frequency < range.second))
            {
                return band;
            }
        }
        return BAND_ANY;
    }

    void AccessPointData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(!this->ssid.empty(), "ssid", this->ssid.as_string());
        tvlist->append_if(this->frequency != 0, "frequency", this->frequency);
        tvlist->append_if(this->flags != 0, "flags", this->flags);
        tvlist->append_if(this->rsn_flags != 0, "rsn_flags", this->rsn_flags);
        tvlist->append_if(this->wpa_flags != 0, "wpa_flags", this->wpa_flags);
        tvlist->append_if(this->auth_type(), "auth_type", this->auth_type());
        tvlist->append_if(!this->hwAddress.empty(), "hwAddress", this->hwAddress);
        tvlist->append_if(this->mode, "ap_mode", core::str::convert_from(this->mode));
        tvlist->append_if(this->maxbitrate != 0, "maxbitrate", this->maxbitrate);
        tvlist->append_if(this->strength != 0, "strength", this->strength);
        tvlist->append_if(this->lastSeen != core::dt::epoch, "lastSeen", this->lastSeen);
    }

    void WiredDeviceData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->speed, "speed", this->speed);
    }

    void WirelessDeviceData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->mode,
                          "ap_mode",
                          core::str::convert_from(this->mode));

        tvlist->append_if(this->bitrate,
                          "bitrate",
                          this->bitrate);

        tvlist->append_if(!this->active_accesspoint.empty(),
                          "active_accesspoint",
                          this->active_accesspoint);

        tvlist->append_if(this->lastScan != core::dt::epoch,
                          "lastScan",
                          this->lastScan);
    }

    std::string DeviceData::key() const
    {
        return this->interface;
    }

    const WiredDeviceData *DeviceData::wired_data() const
    {
        return std::get_if<WiredDeviceData>(&this->specific_data);
    }

    const WirelessDeviceData *DeviceData::wifi_data() const
    {
        return std::get_if<WirelessDeviceData>(&this->specific_data);
    }

    bool DeviceData::is_managed() const
    {
        switch (this->state)
        {
        case NM_DEVICE_STATE_UNKNOWN:
        case NM_DEVICE_STATE_UNMANAGED:
            return false;

        default:
            return true;
        }
    }

    void DeviceData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->type,
                          "type",
                          core::str::convert_from(this->type));

        tvlist->append_if(this->state,
                          "state",
                          core::str::convert_from(this->state));

        tvlist->append_if(this->state_reason,
                          "state_reason",
                          core::str::convert_from(this->state_reason));

        tvlist->append_if(!this->interface.empty(),
                          "interface",
                          this->interface);

        tvlist->append_if(!this->hwAddress.empty(),
                          "hwAddress",
                          this->hwAddress);

        tvlist->append("ip4config",
                       this->ip4config.as_tvlist());

        tvlist->append("ip6config",
                       this->ip6config.as_tvlist());

        tvlist->append_if(this->ip4connectivity,
                          "ip4connectivity",
                          core::str::convert_from(this->ip4connectivity));

        tvlist->append_if(this->ip6connectivity,
                          "ip6connectivity",
                          core::str::convert_from(this->ip6connectivity));

        tvlist->append_if(!this->active_connection.empty(),
                          "active_connection",
                          this->active_connection);

        switch (this->type)
        {
        case NM_DEVICE_TYPE_ETHERNET:
            tvlist->append(
                "wired",
                std::get<WiredDeviceData>(this->specific_data).as_tvlist());
            break;

        case NM_DEVICE_TYPE_WIFI:
            tvlist->append(
                "wifi",
                std::get<WirelessDeviceData>(this->specific_data).as_tvlist());
            break;

        default:
            break;
        }
    }

    void GlobalData::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append_if(this->state,
                          "state",
                          core::str::convert_from(this->state));

        tvlist->append_if(this->connectivity,
                          "connectivity",
                          core::str::convert_from(this->connectivity));

        tvlist->append("wireless_allowed",
                       this->wireless_allowed);

        tvlist->append("wireless_enabled",
                       this->wireless_enabled);

        tvlist->append("wireless_hw_enabled",
                       this->wireless_hardware_enabled);

        tvlist->append("wireless_band_selection",
                       core::str::convert_from(this->wireless_band_selection));
    }

    std::istream &operator>>(std::istream &stream, WirelessBandSelection &band)
    {
        return band_selection_map.from_stream(stream, &band);
    }

    std::ostream &operator<<(std::ostream &stream, WirelessBandSelection band)
    {
        return band_selection_map.to_stream(stream, band, static_cast<uint>(band));
    }

    std::ostream &operator<<(std::ostream &stream, IPConfigMethod method)
    {
        if (method != METHOD_NONE)
        {
            ipconfig_method_map.to_stream(stream, method, static_cast<uint>(method));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, KeyManagement key_mgmt)
    {
        if (key_mgmt != KEY_EMPTY)
        {
            key_mgmt_map.to_stream(stream, key_mgmt, static_cast<uint>(key_mgmt));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, AuthenticationProtocol auth_proto)
    {
        return auth_proto_map.to_stream(stream, auth_proto, static_cast<uint>(auth_proto));
    }

    std::ostream &operator<<(std::ostream &stream, AuthenticationAlgorithm auth_alg)
    {
        if (auth_alg != AUTH_ALG_NONE)
        {
            auth_alg_map.to_stream(stream, auth_alg, static_cast<uint>(auth_alg));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, AuthenticationType auth_type)
    {
        if (auth_type != AUTH_TYPE_NONE)
        {
            auth_type_map.to_stream(stream, auth_type, static_cast<uint>(auth_type));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, EAP_Type eap_type)
    {
        if (eap_type != EAP_NONE)
        {
            eap_type_map.to_stream(stream, eap_type, static_cast<uint>(eap_type));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, EAP_Phase2 eap_phase2)
    {
        if (eap_phase2 != Phase2_NONE)
        {
            eap_phase2_map.to_stream(stream, eap_phase2, static_cast<uint>(eap_phase2));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, FAST_Provisioning provisioning)
    {
        if (provisioning != Provisioning_NONE)
        {
            fast_provisioning_map.to_stream(stream, provisioning, static_cast<uint>(provisioning));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, ConnectionType conn_type)
    {
        if (conn_type != CONN_TYPE_UNKNOWN)
        {
            connection_type_map.to_stream(stream, conn_type, static_cast<uint>(conn_type));
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, AuthenticationData auth_data)
    {
        switch (static_cast<AuthenticationType>(auth_data.index()))
        {
        case AUTH_TYPE_WEP:
            stream << "WEP" << std::get<WEP_Data>(auth_data);
            break;

        case AUTH_TYPE_WPA:
            stream << "WPA" << std::get<WPA_Data>(auth_data);
            break;

        case AUTH_TYPE_EAP:
            stream << "EAP" << std::get<EAP_Data>(auth_data);
            break;

        default:
            break;
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, ConnectionSpecificData conn_data)
    {
        switch (static_cast<ConnectionType>(conn_data.index()))
        {
        case CONN_TYPE_WIRED:
            stream << std::get<WiredConnectionData>(conn_data);
            break;

        case CONN_TYPE_WIRELESS:
            stream << std::get<WirelessConnectionData>(conn_data);
            break;

        default:
            break;
        }
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, DeviceSpecificData dev_data)
    {
        switch (static_cast<NMDeviceType>(dev_data.index()))
        {
        case NM_DEVICE_TYPE_ETHERNET:
            stream << std::get<WiredDeviceData>(dev_data);
            break;

        case NM_DEVICE_TYPE_WIFI:
            stream << std::get<WirelessDeviceData>(dev_data);
            break;

        default:
            break;
        }
        return stream;
    }

    const core::types::SymbolMap<NMState> state_map = {
        {NM_STATE_ASLEEP, "asleep"},
        {NM_STATE_DISCONNECTED, "disconnected"},
        {NM_STATE_DISCONNECTING, "disconnecting"},
        {NM_STATE_CONNECTING, "connecting"},
        {NM_STATE_CONNECTED_LOCAL, "link-local"},
        {NM_STATE_CONNECTED_SITE, "site-local"},
        {NM_STATE_CONNECTED_GLOBAL, "global"},
    };

    const core::types::SymbolMap<IPConfigMethod> ipconfig_method_map = {
        {METHOD_DISABLED, "disabled"},
        {METHOD_AUTO, "auto"},
        {METHOD_DHCP, "dhcp"},
        {METHOD_MANUAL, "manual"},
        {METHOD_LINK_LOCAL, "link-local"},
    };

    const core::types::SymbolMap<WirelessBandSelection> band_selection_map = {
        {BAND_ANY, "any"},
        {BAND_A, "a"},
        {BAND_BG, "bg"},
    };

    const core::types::SymbolMap<ConnectionType> connection_type_map = {
        {CONN_TYPE_WIRED, NM_SETTING_WIRED_SETTING_NAME},
        {CONN_TYPE_WIRELESS, NM_SETTING_WIRELESS_SETTING_NAME},
    };

    const core::types::SymbolMap<NMWepKeyType> wep_key_type_map = {
        {NM_WEP_KEY_TYPE_KEY, "key"},
        {NM_WEP_KEY_TYPE_PASSPHRASE, "passphrase"},
    };

    const core::types::SymbolMap<NMDeviceType> device_type_map = {
        {NM_DEVICE_TYPE_ETHERNET, "ethernet"},
        {NM_DEVICE_TYPE_WIFI, "wifi"},
        {NM_DEVICE_TYPE_BT, "bluetooth"},
        {NM_DEVICE_TYPE_OLPC_MESH, "oplc_mesh"},
        {NM_DEVICE_TYPE_WIMAX, "wimax"},
        {NM_DEVICE_TYPE_MODEM, "modem"},
        {NM_DEVICE_TYPE_INFINIBAND, "infiniband"},
        {NM_DEVICE_TYPE_BOND, "bond"},
        {NM_DEVICE_TYPE_ADSL, "adsl"},
        {NM_DEVICE_TYPE_BRIDGE, "bridge"},
        {NM_DEVICE_TYPE_VLAN, "vlan"},
        {NM_DEVICE_TYPE_GENERIC, "generic"},
        {NM_DEVICE_TYPE_TEAM, "team"},
        {NM_DEVICE_TYPE_TUN, "tun"},
        {NM_DEVICE_TYPE_IP_TUNNEL, "iptunnel"},
        {NM_DEVICE_TYPE_MACVLAN, "macvlan"},
        {NM_DEVICE_TYPE_VXLAN, "vxlan"},
        {NM_DEVICE_TYPE_VETH, "veth"},
        {NM_DEVICE_TYPE_MACSEC, "macsec"},
        {NM_DEVICE_TYPE_DUMMY, "dummy"},
        {NM_DEVICE_TYPE_PPP, "ppp"},
        {NM_DEVICE_TYPE_OVS_INTERFACE, "ovs_interface"},
        {NM_DEVICE_TYPE_OVS_PORT, "ovs_port"},
        {NM_DEVICE_TYPE_OVS_BRIDGE, "ovs_bridge"},
        {NM_DEVICE_TYPE_WPAN, "wpan"},
        {NM_DEVICE_TYPE_6LOWPAN, "6lowpan"},
        {NM_DEVICE_TYPE_WIREGUARD, "wireguard"},
        {NM_DEVICE_TYPE_WIFI_P2P, "wifi_p2p"},
    };

    const core::types::SymbolMap<NMDeviceState> device_state_map = {
        {NM_DEVICE_STATE_UNMANAGED, "unmanaged"},
        {NM_DEVICE_STATE_UNAVAILABLE, "unavailable"},
        {NM_DEVICE_STATE_DISCONNECTED, "disconnected"},
        {NM_DEVICE_STATE_PREPARE, "preparing"},
        {NM_DEVICE_STATE_CONFIG, "configuring"},
        {NM_DEVICE_STATE_NEED_AUTH, "needs_authentication"},
        {NM_DEVICE_STATE_IP_CONFIG, "configuring"},
        {NM_DEVICE_STATE_IP_CHECK, "ip_check"},
        {NM_DEVICE_STATE_SECONDARIES, "secondaries"},
        {NM_DEVICE_STATE_ACTIVATED, "activated"},
        {NM_DEVICE_STATE_DEACTIVATING, "deactivating"},
        {NM_DEVICE_STATE_FAILED, "failed"},
    };

    const core::types::SymbolMap<NMDeviceStateReason> device_state_reason_map = {
        {NM_DEVICE_STATE_REASON_NONE, "none"},
        {NM_DEVICE_STATE_REASON_UNKNOWN, "unknown"},
        {NM_DEVICE_STATE_REASON_NOW_MANAGED, "now_managed"},
        {NM_DEVICE_STATE_REASON_NOW_UNMANAGED, "now_unmanaged"},
        {NM_DEVICE_STATE_REASON_CONFIG_FAILED, "config_failed"},
        {NM_DEVICE_STATE_REASON_IP_CONFIG_UNAVAILABLE, "ip_config_unavailable"},
        {NM_DEVICE_STATE_REASON_IP_CONFIG_EXPIRED, "ip_config_expired"},
        {NM_DEVICE_STATE_REASON_NO_SECRETS, "no_secrets"},
        {NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT, "supplicant_disconnect"},
        {NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED, "supplicant_config_failed"},
        {NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED, "supplicant_failed"},
        {NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT, "supplicant_timeout"},
        {NM_DEVICE_STATE_REASON_PPP_START_FAILED, "ppp_start_failed"},
        {NM_DEVICE_STATE_REASON_PPP_DISCONNECT, "ppp_disconnect"},
        {NM_DEVICE_STATE_REASON_PPP_FAILED, "ppp_failed"},
        {NM_DEVICE_STATE_REASON_DHCP_START_FAILED, "dhcp_start_failed"},
        {NM_DEVICE_STATE_REASON_DHCP_ERROR, "dhcp_error"},
        {NM_DEVICE_STATE_REASON_DHCP_FAILED, "dhcp_failed"},
        {NM_DEVICE_STATE_REASON_SHARED_START_FAILED, "shared_start_failed"},
        {NM_DEVICE_STATE_REASON_SHARED_FAILED, "shared_failed"},
        {NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED, "autoip_start_failed"},
        {NM_DEVICE_STATE_REASON_AUTOIP_ERROR, "autoip_error"},
        {NM_DEVICE_STATE_REASON_AUTOIP_FAILED, "autoip_failed"},
        {NM_DEVICE_STATE_REASON_MODEM_BUSY, "modem_busy"},
        {NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE, "modem_no_dial_tone"},
        {NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER, "modem_no_carrier"},
        {NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT, "modem_dial_timeout"},
        {NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED, "modem_dial_failed"},
        {NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED, "modem_init_failed"},
        {NM_DEVICE_STATE_REASON_GSM_APN_FAILED, "gsm_apn_failed"},
        {NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING, "gsm_registration_not_searching"},
        {NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED, "gsm_registration_denied"},
        {NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT, "gsm_registration_timeout"},
        {NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED, "gsm_registration_failed"},
        {NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED, "gsm_pin_check_failed"},
        {NM_DEVICE_STATE_REASON_FIRMWARE_MISSING, "firmware_missing"},
        {NM_DEVICE_STATE_REASON_REMOVED, "removed"},
        {NM_DEVICE_STATE_REASON_SLEEPING, "sleeping"},
        {NM_DEVICE_STATE_REASON_CONNECTION_REMOVED, "connection_removed"},
        {NM_DEVICE_STATE_REASON_USER_REQUESTED, "user_requested"},
        {NM_DEVICE_STATE_REASON_CARRIER, "carrier"},
        {NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED, "connection_assumed"},
        {NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE, "supplicant_available"},
        {NM_DEVICE_STATE_REASON_MODEM_NOT_FOUND, "modem_not_found"},
        {NM_DEVICE_STATE_REASON_BT_FAILED, "bt_failed"},
        {NM_DEVICE_STATE_REASON_GSM_SIM_NOT_INSERTED, "gsm_sim_not_inserted"},
        {NM_DEVICE_STATE_REASON_GSM_SIM_PIN_REQUIRED, "gsm_sim_pin_required"},
        {NM_DEVICE_STATE_REASON_GSM_SIM_PUK_REQUIRED, "gsm_sim_puk_required"},
        {NM_DEVICE_STATE_REASON_GSM_SIM_WRONG, "gsm_sim_wrong"},
        {NM_DEVICE_STATE_REASON_INFINIBAND_MODE, "infiniband_mode"},
        {NM_DEVICE_STATE_REASON_DEPENDENCY_FAILED, "dependency_failed"},
        {NM_DEVICE_STATE_REASON_BR2684_FAILED, "br2684_failed"},
        {NM_DEVICE_STATE_REASON_MODEM_MANAGER_UNAVAILABLE, "modem_manager_unavailable"},
        {NM_DEVICE_STATE_REASON_SSID_NOT_FOUND, "ssid_not_found"},
        {NM_DEVICE_STATE_REASON_SECONDARY_CONNECTION_FAILED, "secondary_connection_failed"},
        {NM_DEVICE_STATE_REASON_DCB_FCOE_FAILED, "dcb_fcoe_failed"},
        {NM_DEVICE_STATE_REASON_TEAMD_CONTROL_FAILED, "teamd_control_failed"},
        {NM_DEVICE_STATE_REASON_MODEM_FAILED, "modem_failed"},
        {NM_DEVICE_STATE_REASON_MODEM_AVAILABLE, "modem_available"},
        {NM_DEVICE_STATE_REASON_SIM_PIN_INCORRECT, "sim_pin_incorrect"},
        {NM_DEVICE_STATE_REASON_NEW_ACTIVATION, "new_activation"},
        {NM_DEVICE_STATE_REASON_PARENT_CHANGED, "parent_changed"},
        {NM_DEVICE_STATE_REASON_PARENT_MANAGED_CHANGED, "parent_managed_changed"},
        {NM_DEVICE_STATE_REASON_OVSDB_FAILED, "ovsdb_failed"},
        {NM_DEVICE_STATE_REASON_IP_ADDRESS_DUPLICATE, "ip_address_duplicate"},
        {NM_DEVICE_STATE_REASON_IP_METHOD_UNSUPPORTED, "ip_method_unsupported"},
        {NM_DEVICE_STATE_REASON_SRIOV_CONFIGURATION_FAILED, "sriov_configuration_failed"},
        {NM_DEVICE_STATE_REASON_PEER_NOT_FOUND, "peer_not_found"},
    };

    const core::types::SymbolMap<NM80211Mode> ap_mode_map = {
        {NM_802_11_MODE_ADHOC, NM_SETTING_WIRELESS_MODE_ADHOC},
        {NM_802_11_MODE_INFRA, NM_SETTING_WIRELESS_MODE_INFRA},
        {NM_802_11_MODE_AP, NM_SETTING_WIRELESS_MODE_AP},
#ifdef NM_SETTING_WIRELESS_MODE_MESH
        {NM_802_11_MODE_MESH, NM_SETTING_WIRELESS_MODE_MESH},
#endif
    };

    const core::types::SymbolMap<KeyManagement> key_mgmt_map = {
        {KEY_WEP, "none"},
        {KEY_IEEE8021X, "ieee8021x"},
        {KEY_PSK, "wpa-psk"},
        {KEY_SAE, "sae"},
        {KEY_OWE, "owe"},
        {KEY_EAP, "wpa-eap"},
    };

    const core::types::SymbolMap<AuthenticationProtocol> auth_proto_map = {
        {AUTH_PROTO_WPA, "wpa"},
        {AUTH_PROTO_RSN, "rsn"},
    };

    const core::types::SymbolMap<AuthenticationAlgorithm> auth_alg_map = {
        {AUTH_ALG_WEP_OPEN, "open"},
        {AUTH_ALG_WEP_SHARED, "shared"},
        {AUTH_ALG_LEAP, "leap"},
    };

    const core::types::SymbolMap<AuthenticationType> auth_type_map = {
        {AUTH_TYPE_WEP, "wep"},
        {AUTH_TYPE_WPA, "wpa"},
        {AUTH_TYPE_EAP, "eap"},
        {AUTH_TYPE_UNKNOWN, "unknown"},
    };

    const core::types::SymbolMap<EAP_Type> eap_type_map = {
        {EAP_MD5, "md5"},
        {EAP_TLS, "tls"},
        {EAP_TTLS, "ttls"},
        {EAP_FAST, "fast"},
        {EAP_LEAP, "leap"},
        {EAP_PEAP, "peap"},
        {EAP_PWD, "pwd"},
    };

    const core::types::SymbolMap<EAP_Phase2> eap_phase2_map = {
        {Phase2_MD5, "md5"},
        {Phase2_GTC, "gtc"},
        {Phase2_PAP, "pap"},
        {Phase2_CHAP, "chap"},
        {Phase2_MSCHAP, "mschap"},
        {Phase2_MSCHAPv2, "mschapv2"},
        {Phase2_MSCHAPv2_NO_EAP, "mschapv2_no_eap"},
    };

    const core::types::SymbolMap<FAST_Provisioning> fast_provisioning_map = {
        {Provisioning_NONE, "disabled"},
        {Provisioning_ANONYMOUS, "anonymous"},
        {Provisioning_AUTHENTICATED, "authenticated"},
        {Provisioning_ANY, "any"},
    };

    const core::types::SymbolMap<NMActiveConnectionState> ac_state_map = {
        {NM_ACTIVE_CONNECTION_STATE_ACTIVATING, "activating"},
        {NM_ACTIVE_CONNECTION_STATE_ACTIVATED, "activated"},
        {NM_ACTIVE_CONNECTION_STATE_DEACTIVATING, "deactivating"},
        {NM_ACTIVE_CONNECTION_STATE_DEACTIVATED, "deactivated"},
    };

    const core::types::SymbolMap<NMActiveConnectionStateReason> ac_reason_map = {
        {NM_ACTIVE_CONNECTION_STATE_REASON_NONE, "none"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_USER_DISCONNECTED, "user_disconnected"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_DISCONNECTED, "device_disconnected"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_STOPPED, "service_stopped"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_IP_CONFIG_INVALID, "ip_config_invalid"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_CONNECT_TIMEOUT, "connect_timeout"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_START_TIMEOUT, "service_start_timeout"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_SERVICE_START_FAILED, "service_start_failed"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_NO_SECRETS, "no_secrets"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_LOGIN_FAILED, "login_failed"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_CONNECTION_REMOVED, "connection_removed"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_DEPENDENCY_FAILED, "dependency_failed"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_REALIZE_FAILED, "device_realize_failed"},
        {NM_ACTIVE_CONNECTION_STATE_REASON_DEVICE_REMOVED, "device_removed"},
    };

    const core::types::SymbolMap<NMConnectivityState> connectivity_state_map = {
        {NM_CONNECTIVITY_NONE, "none"},
        {NM_CONNECTIVITY_PORTAL, "portal"},
        {NM_CONNECTIVITY_LIMITED, "limited"},
        {NM_CONNECTIVITY_FULL, "full"},
    };

}  // namespace netconfig

std::ostream &operator<<(std::ostream &stream, NM80211Mode mode)
{
    if (mode != NM_802_11_MODE_UNKNOWN)
    {
        try
        {
            stream << netconfig::ap_mode_map.at(mode);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(mode);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMState state)
{
    if (state != NM_STATE_UNKNOWN)
    {
        try
        {
            stream << netconfig::state_map.at(state);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(state);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMWepKeyType key_type)
{
    if (key_type != NM_WEP_KEY_TYPE_UNKNOWN)
    {
        try
        {
            stream << netconfig::wep_key_type_map.at(key_type);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(key_type);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMDeviceType dev_type)
{
    if (dev_type != NM_DEVICE_TYPE_UNKNOWN)
    {
        try
        {
            stream << netconfig::device_type_map.at(dev_type);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(dev_type);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMDeviceState dev_state)
{
    if (dev_state != NM_DEVICE_STATE_UNKNOWN)
    {
        try
        {
            stream << netconfig::device_state_map.at(dev_state);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(dev_state);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMDeviceStateReason dev_state_reason)
{
    if (dev_state_reason != NM_DEVICE_STATE_REASON_NONE)
    {
        try
        {
            stream << netconfig::device_state_reason_map.at(dev_state_reason);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(dev_state_reason);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMActiveConnectionState state)
{
    if (state != NM_ACTIVE_CONNECTION_STATE_UNKNOWN)
    {
        try
        {
            stream << netconfig::ac_state_map.at(state);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(state);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMActiveConnectionStateReason reason)
{
    if (reason != NM_ACTIVE_CONNECTION_STATE_REASON_UNKNOWN)
    {
        try
        {
            stream << netconfig::ac_reason_map.at(reason);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(reason);
        }
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, NMConnectivityState state)
{
    if (state != NM_CONNECTIVITY_UNKNOWN)
    {
        try
        {
            stream << netconfig::connectivity_state_map.at(state);
        }
        catch (std::out_of_range)
        {
            stream << static_cast<uint>(state);
        }
    }
    return stream;
}
