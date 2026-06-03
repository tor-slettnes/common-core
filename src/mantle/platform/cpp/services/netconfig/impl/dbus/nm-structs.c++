/// -*- c++ -*-
//==============================================================================
/// @file nm-structs.c++
/// @brief Network Manager object wrapper
/// @author Tor Slettnes
//==============================================================================

#include "nm-structs.h++"
#include "glib-variant.h++"
#include "logging/logging.h++"

#include <NetworkManager.h>

#include <glibmm/refptr.h>
#include <glibmm/variant.h>
#include <glibmm/variantdict.h>
#include <giomm/inetaddress.h>

#include <arpa/inet.h>

#define NM_SETTING_IP_CONFIG_ADDRESS_DATA "address-data"

#define MAX_IP4_STRING_SIZE 4 * 4
#define MAX_IP6_STRING_SIZE 8 * 5
// #define NM_SETTING_IP_CONFIG_GATEWAY      "gateway"
// #define NM_SETTING_IP_CONFIG_DNS          "dns"

namespace cc::platform::netconfig::dbus
{
    void extract_addressdata(
        const cc::glib::VariantMaps& configs,
        AddressVector* addresses)
    {
        addresses->clear();
        for (const auto& config : configs)
        {
            auto& conf = addresses->emplace_back();
            cc::glib::extract_value(config, "address", &conf.address);
            cc::glib::extract_value(config, "prefix", &conf.prefixlength);
        }
    }

    static std::vector<std::string> wep_key_names = {
        NM_SETTING_WIRELESS_SECURITY_WEP_KEY0,
        NM_SETTING_WIRELESS_SECURITY_WEP_KEY1,
        NM_SETTING_WIRELESS_SECURITY_WEP_KEY2,
        NM_SETTING_WIRELESS_SECURITY_WEP_KEY3,
    };

    core::types::ByteVector file_url(const fs::path& path)
    {
        return URI_PREFIX_FILE + path.string() + "\0"s;
    }

    namespace connection
    {
        //======================================================================
        // Methods to extract from Settings::Connection maps

        void extract_connection(
            const cc::glib::VariantMap& map,
            ConnectionData* data)
        {
            cc::glib::extract_value(
                map,
                NM_SETTING_CONNECTION_ID,
                &data->id);

            cc::glib::extract_value(
                map,
                NM_SETTING_CONNECTION_UUID,
                &data->uuid);

            cc::glib::extract_value(
                map,
                NM_SETTING_CONNECTION_INTERFACE_NAME,
                &data->interface);
        }

        void extract_wired(
            const cc::glib::VariantMap& map,
            WiredConnectionData* wired)
        {
            cc::glib::extract_value(
                map,
                NM_SETTING_WIRED_AUTO_NEGOTIATE,
                &wired->auto_negotiate);
        }

        void extract_wireless(
            const cc::glib::VariantMap& map,
            WirelessConnectionData* wireless)
        {
            cc::glib::extract_value<core::types::Bytes>(
                map,
                NM_SETTING_WIRELESS_SSID,
                &wireless->ssid);

            cc::glib::extract_value(
                map,
                NM_SETTING_WIRELESS_HIDDEN,
                &wireless->hidden);

            cc::glib::extract_value(
                map,
                NM_SETTING_WIRELESS_TX_POWER,
                &wireless->tx_power);

            WirelessBandSelection band = BAND_ANY;
            if (cc::glib::extract_mapped(
                    map,
                    band_selection_map,
                    NM_SETTING_WIRELESS_BAND,
                    &band))
            {
                wireless->band = band;
            }
            else
            {
                wireless->band.reset();
            }

            cc::glib::extract_mapped(
                map,
                ap_mode_map,
                NM_SETTING_WIRELESS_MODE,
                &wireless->mode);
        }

        WEP_Data
        auth_data_wep(const cc::glib::VariantMap& map)
        {
            WEP_Data wep;
            cc::glib::extract_mapped(
                map,
                auth_alg_map,
                NM_SETTING_WIRELESS_SECURITY_AUTH_ALG,
                &wep.auth_alg);

            for (uint i = 0; i < wep_key_names.size(); i++)
            {
                std::string value;
                cc::glib::extract_value(
                    map,
                    wep_key_names.at(i),
                    &value);

                wep.keys[i] = core::types::ByteVector::from_string(value);
            }

            cc::glib::extract_value(
                map,
                NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX,
                &wep.key_idx);

            cc::glib::extract_value(
                map,
                NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
                &wep.key_type);

            return wep;
        }

        WPA_Data
        auth_data_wpa(const cc::glib::VariantMap& map)
        {
            WPA_Data wpa;
            cc::glib::extract_value(
                map,
                NM_SETTING_WIRELESS_SECURITY_PSK,
                &wpa.psk);
            return wpa;
        }

        EAP_Data
        auth_data_eap(const cc::glib::VariantMap& map)
        {
            EAP_Data eap;
            cc::glib::extract_mapped(
                map,
                auth_alg_map,
                NM_SETTING_WIRELESS_SECURITY_AUTH_ALG,
                &eap.auth_alg);
            return eap;
        }

        void extract_wireless_security(
            const cc::glib::VariantMap& map,
            WirelessConnectionData* wireless)
        {
            std::string key_mgmt_name;
            if (cc::glib::extract_value(
                    map,
                    NM_SETTING_WIRELESS_SECURITY_KEY_MGMT,
                    &key_mgmt_name))
            {
                wireless->key_mgmt = key_mgmt_map.from_string(key_mgmt_name, KEY_EMPTY);
            }

            std::vector<std::string> protos;
            if (cc::glib::extract_value(
                    map, NM_SETTING_WIRELESS_SECURITY_PROTO, &protos))
            {
                for (const std::string& proto : protos)
                {
                    if (auto mask = auth_proto_map.try_from_string(proto))
                    {
                        wireless->auth_protos |= mask.value();
                    }
                }
            }

            switch (wireless->key_mgmt)
            {
            case KEY_WEP:
                wireless->auth = auth_data_wep(map);
                break;

            case KEY_PSK:
            case KEY_SAE:
                wireless->auth = auth_data_wpa(map);
                break;

            case KEY_IEEE8021X:
            case KEY_EAP:
                wireless->auth = auth_data_eap(map);
                break;

            default:
                logf_notice("Unsupported key management scheme: %r, index %d",
                            key_mgmt_name,
                            static_cast<int>(wireless->key_mgmt));
            }
        }

        void extract_eap(
            const cc::glib::VariantMap& map,
            EAP_Data* data)
        {
            std::vector<std::string> eap_schemes;

            if (cc::glib::extract_value(
                    map, NM_SETTING_802_1X_EAP, &eap_schemes))
            {
                data->eap_type = eap_type_map.from_string(eap_schemes.at(0), EAP_NONE);
            }

            cc::glib::extract_mapped(
                map,
                eap_phase2_map,
                NM_SETTING_802_1X_PHASE2_AUTH,
                &data->eap_phase2);

            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_ANONYMOUS_IDENTITY,
                &data->anonymous_identity);

            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_IDENTITY,
                &data->identity);

            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_PASSWORD,
                &data->password);

            cc::glib::extract_filepath(
                map,
                NM_SETTING_802_1X_CA_CERT,
                &data->ca_cert);

            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_CLIENT_CERT_PASSWORD,
                &data->ca_cert_password);

            cc::glib::extract_filepath(
                map,
                NM_SETTING_802_1X_CLIENT_CERT,
                &data->client_cert);

            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_CLIENT_CERT_PASSWORD,
                &data->client_cert_password);

            cc::glib::extract_filepath(
                map,
                NM_SETTING_802_1X_PAC_FILE,
                &data->pac_file);

            std::string s;
            cc::glib::extract_value(
                map,
                NM_SETTING_802_1X_PHASE1_FAST_PROVISIONING,
                &s);

            if (s.size())
            {
                uint n = core::str::convert_to<uint>(s) + Provisioning_DISABLED;
                data->fast_provisioning = static_cast<FAST_Provisioning>(n);
            }
        }

        void extract_ipconfig(
            const cc::glib::VariantMap& map,
            IPConfigData* ipconfig)
        {
            cc::glib::extract_mapped(
                map,
                ipconfig_method_map,
                NM_SETTING_IP_CONFIG_METHOD,
                &ipconfig->method);

            cc::glib::VariantMaps settings;
            cc::glib::extract_value(
                map,
                NM_SETTING_IP_CONFIG_ADDRESS_DATA,
                &settings);

            extract_addressdata(settings, &ipconfig->address_data);

            cc::glib::extract_value(
                map,
                NM_SETTING_IP_CONFIG_GATEWAY,
                &ipconfig->gateway);

            cc::glib::extract_value(
                map,
                NM_SETTING_IP_CONFIG_DNS_SEARCH,
                &ipconfig->searches);
        }

        void extract_ip4config(
            const cc::glib::VariantMap& map,
            IPConfigData* ipconfig)
        {
            extract_ipconfig(map, ipconfig);
            std::vector<std::uint32_t> addrs;
            cc::glib::extract_value(
                map,
                NM_SETTING_IP_CONFIG_DNS,
                &addrs);

            ipconfig->dns.clear();
            ipconfig->dns.reserve(addrs.size());
            for (std::uint32_t addr : addrs)
            {
                char buf[INET_ADDRSTRLEN];
                if (inet_ntop(AF_INET, &addr, buf, sizeof(buf)))
                {
                    ipconfig->dns.push_back(buf);
                }
            }
        }

        void extract_ip6config(
            const cc::glib::VariantMap& map,
            IPConfigData* ipconfig)
        {
            extract_ipconfig(map, ipconfig);

            std::vector<std::vector<std::uint8_t>> addrs;
            cc::glib::extract_value(
                map,
                NM_SETTING_IP_CONFIG_DNS,
                &addrs);

            ipconfig->dns.clear();
            ipconfig->dns.reserve(addrs.size());
            for (const std::vector<std::uint8_t>& bytes : addrs)
            {
                char buf[INET6_ADDRSTRLEN];
                if (inet_ntop(AF_INET6, bytes.data(), buf, sizeof(buf)))
                {
                    ipconfig->dns.push_back(buf);
                }
            }
        }

        void extract_settings(
            const cc::glib::SettingsMap& map,
            ConnectionData* data)
        {
            auto it = map.find(NM_SETTING_CONNECTION_SETTING_NAME);
            if (it != map.end())
            {
                extract_connection(it->second, data);

                if ((it = map.find(NM_SETTING_WIRED_SETTING_NAME)) != map.end())
                {
                    WiredConnectionData wired;
                    extract_wired(it->second, &wired);
                    data->specific_data = wired;
                }
                else if ((it = map.find(NM_SETTING_WIRELESS_SETTING_NAME)) != map.end())
                {
                    WirelessConnectionData wireless;
                    extract_wireless(it->second, &wireless);

                    if ((it = map.find(NM_SETTING_WIRELESS_SECURITY_SETTING_NAME)) != map.end())
                    {
                        extract_wireless_security(it->second, &wireless);
                    }

                    if ((it = map.find(NM_SETTING_802_1X_SETTING_NAME)) != map.end())
                    {
                        if (auto* auth = std::get_if<EAP_Data>(&wireless.auth))
                        {
                            extract_eap(it->second, auth);
                        }
                    }
                    data->specific_data = wireless;
                }

                if ((it = map.find(NM_SETTING_IP4_CONFIG_SETTING_NAME)) != map.end())
                {
                    extract_ip4config(it->second, &data->ip4config);
                }

                if ((it = map.find(NM_SETTING_IP6_CONFIG_SETTING_NAME)) != map.end())
                {
                    extract_ip6config(it->second, &data->ip6config);
                }
            }
        }

        void extract_settings(const cc::glib::SettingsContainer& container,
                              ConnectionData* data)
        {
            extract_settings(cc::glib::variant_cast<cc::glib::SettingsMap>(container), data);
        }

        //======================================================================
        // Methods to build Settings::Connection maps

        void insert_connection(
            const ConnectionData& data,
            cc::glib::VariantMap* map)
        {
            cc::glib::insert_value(
                map,
                NM_SETTING_CONNECTION_ID,
                data.id);

            cc::glib::insert_value(
                map,
                NM_SETTING_CONNECTION_UUID,
                data.uuid);

            cc::glib::insert_value(
                map,
                NM_SETTING_CONNECTION_INTERFACE_NAME,
                data.interface);

            cc::glib::insert_mapped(
                map,
                connection_type_map,
                NM_SETTING_CONNECTION_TYPE,
                data.type());
        }

        void insert_wired(
            const WiredConnectionData& wired,
            cc::glib::VariantMap* map)
        {
            cc::glib::insert_value(
                map,
                NM_SETTING_WIRED_AUTO_NEGOTIATE,
                wired.auto_negotiate);
        }

        void insert_wireless(
            const WirelessConnectionData& wireless,
            cc::glib::VariantMap* map)
        {
            cc::glib::insert_value<core::types::Bytes>(
                map,
                NM_SETTING_WIRELESS_SSID,
                wireless.ssid);

            cc::glib::insert_value(
                map,
                NM_SETTING_WIRELESS_HIDDEN,
                wireless.hidden);

            cc::glib::insert_value(
                map,
                NM_SETTING_WIRELESS_TX_POWER,
                wireless.tx_power);

            if (wireless.band.value_or(BAND_ANY) != BAND_ANY)
            {
                cc::glib::insert_mapped(
                    map,
                    band_selection_map,
                    NM_SETTING_WIRELESS_BAND,
                    wireless.band.value());
            }

            if (wireless.key_mgmt_type() != KEY_EMPTY)
            {
                cc::glib::insert_value(
                    map,
                    "security",
                    NM_SETTING_WIRELESS_SECURITY_SETTING_NAME ""s);
            }
        }

        void insert_wireless_security(
            const WirelessConnectionData& wireless,
            cc::glib::VariantMap* map)
        {
            cc::glib::insert_mapped(
                map,
                key_mgmt_map,
                NM_SETTING_WIRELESS_SECURITY_KEY_MGMT,
                wireless.key_mgmt_type());

            if (wireless.auth_protos)
            {
                std::vector<Glib::ustring> protos;
                for (const auto& [mask, proto] : auth_proto_map)
                {
                    if (wireless.auth_protos & mask)
                    {
                        protos.push_back(proto);
                    }
                }
                cc::glib::insert_value(
                    map,
                    NM_SETTING_WIRELESS_SECURITY_PROTO,
                    protos);
            }

            if (auto* wep = std::get_if<WEP_Data>(&wireless.auth))
            {
                if (wep->auth_alg != AUTH_ALG_NONE)
                {
                    cc::glib::insert_mapped(
                        map,
                        auth_alg_map,
                        NM_SETTING_WIRELESS_SECURITY_AUTH_ALG,
                        wep->auth_alg);
                }

                for (uint i = 0; i < wep_key_names.size(); i++)
                {
                    cc::glib::insert_value(
                        map,
                        wep_key_names.at(i),
                        wep->keys.at(i).to_string());
                }

                cc::glib::insert_value(
                    map,
                    NM_SETTING_WIRELESS_SECURITY_WEP_TX_KEYIDX,
                    wep->key_idx);

                if (wep->key_type != NM_WEP_KEY_TYPE_UNKNOWN)
                {
                    cc::glib::insert_value<uint>(
                        map,
                        NM_SETTING_WIRELESS_SECURITY_WEP_KEY_TYPE,
                        wep->key_type);
                }
            }

            else if (auto* wpa = std::get_if<WPA_Data>(&wireless.auth))
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_WIRELESS_SECURITY_PSK,
                    std::string(wpa->psk));
            }

            else if (auto* eap = std::get_if<EAP_Data>(&wireless.auth))
            {
                cc::glib::insert_mapped(
                    map,
                    auth_alg_map,
                    NM_SETTING_WIRELESS_SECURITY_AUTH_ALG,
                    eap->auth_alg);
            }
        }

        void insert_eap(
            const EAP_Data& data,
            cc::glib::VariantMap* map)
        {
            if (const auto& eap_scheme = eap_type_map.try_to_string(data.eap_type))
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_EAP,
                    std::vector<Glib::ustring>({*eap_scheme}));
            }

            if (data.eap_phase2 != Phase2_NONE)
            {
                cc::glib::insert_mapped(
                    map,
                    eap_phase2_map,
                    NM_SETTING_802_1X_PHASE2_AUTH,
                    data.eap_phase2);
            }

            if (!data.anonymous_identity.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_ANONYMOUS_IDENTITY,
                    data.anonymous_identity);
            }

            if (!data.identity.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_IDENTITY,
                    data.identity);
            }

            if (!data.password.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_PASSWORD,
                    std::string(data.password));
            }

            if (!data.ca_cert.empty())
            {
                cc::glib::insert_value<core::types::Bytes>(
                    map,
                    NM_SETTING_802_1X_CA_CERT,
                    file_url(data.ca_cert));
            }

            if (!data.ca_cert_password.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_CA_CERT_PASSWORD,
                    std::string(data.ca_cert_password));
            }

            if (!data.client_cert.empty())
            {
                cc::glib::insert_value<core::types::Bytes>(
                    map,
                    NM_SETTING_802_1X_CLIENT_CERT,
                    file_url(data.client_cert));
            }

            if (!data.client_cert_password.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_CLIENT_CERT_PASSWORD,
                    std::string(data.client_cert_password));
            }

            if (!data.pac_file.empty())
            {
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_PAC_FILE,
                    std::string(data.pac_file.string()));
            }

            if (data.fast_provisioning)
            {
                uint n = static_cast<uint>(data.fast_provisioning) - Provisioning_DISABLED;
                cc::glib::insert_value(
                    map,
                    NM_SETTING_802_1X_PHASE1_FAST_PROVISIONING,
                    core::str::convert_from(n));
            }
        }

        Glib::VariantBase wrap_address_data(const std::vector<AddressData>& data)
        {
            GVariantBuilder builder;
            g_variant_builder_init(&builder, G_VARIANT_TYPE("aa{sv}"));
            for (const auto& item : data)
            {
                g_variant_builder_open(&builder, G_VARIANT_TYPE("a{sv}"));

                //g_variant_builder_open(&builder, G_VARIANT_TYPE("{sv}"));
                g_variant_builder_add(&builder,
                                      "{sv}",
                                      "address",
                                      g_variant_new_string(item.address.data()));
                g_variant_builder_add(&builder,
                                      "{sv}",
                                      "prefix",
                                      g_variant_new_uint32(item.prefixlength));

                g_variant_builder_close(&builder);
            }
            return Glib::wrap(g_variant_builder_end(&builder));
        }

        Glib::VariantBase wrap_string_vector(const std::vector<std::string>& data)
        {
            GVariantBuilder builder;
            g_variant_builder_init(&builder, G_VARIANT_TYPE("as"));
            for (const std::string& s : data)
            {
                g_variant_builder_add(&builder,
                                      "s",
                                      g_variant_new_string(s.data()));
            }
            return Glib::wrap(g_variant_builder_end(&builder));
        }

        void insert_ipconfig(
            const IPConfigData& ipconfig,
            cc::glib::VariantMap* map)
        {
            cc::glib::insert_mapped(
                map,
                ipconfig_method_map,
                NM_SETTING_IP_CONFIG_METHOD,
                ipconfig.method);

            map->emplace(
                NM_SETTING_IP_CONFIG_ADDRESS_DATA,
                wrap_address_data(ipconfig.address_data));

            cc::glib::insert_value(
                map,
                NM_SETTING_IP_CONFIG_GATEWAY,
                ipconfig.gateway);

            std::vector<Glib::ustring> searches(ipconfig.searches.begin(),
                                                ipconfig.searches.end());
            cc::glib::insert_value(
                map,
                NM_SETTING_IP_CONFIG_DNS_SEARCH,
                searches);
        }

        void insert_ip4config(
            const IPConfigData& ipconfig,
            cc::glib::VariantMap* map)
        {
            insert_ipconfig(ipconfig, map);

            // Convert IP address string to binary form
            std::vector<std::uint32_t> addrs;
            addrs.reserve(ipconfig.dns.size());

            for (const IPAddress dns : ipconfig.dns)
            {
                std::uint32_t addr = 0;
                inet_pton(AF_INET, dns.data(), &addr);
                addrs.push_back(addr);
            }

            cc::glib::insert_value(
                map,
                NM_SETTING_IP_CONFIG_DNS,
                addrs);
        }

        void insert_ip6config(
            const IPConfigData& ipconfig,
            cc::glib::VariantMap* map)
        {
            insert_ipconfig(ipconfig, map);

            // Convert IP6 address string to binary form
            std::vector<std::vector<std::uint8_t>> addrs;
            addrs.reserve(ipconfig.dns.size());

            for (const IPAddress dns : ipconfig.dns)
            {
                std::vector<std::uint8_t> bytes(16);
                inet_pton(AF_INET6, dns.data(), bytes.data());
                addrs.push_back(bytes);
            }

            cc::glib::insert_value(
                map,
                NM_SETTING_IP_CONFIG_DNS,
                addrs);
        }

        void insert_settings(
            const ConnectionData& data,
            cc::glib::SettingsMap* settings)
        {
            insert_connection(data,
                              &(*settings)[NM_SETTING_CONNECTION_SETTING_NAME]);

            if (auto* wired = std::get_if<WiredConnectionData>(&data.specific_data))
            {
                insert_wired(*wired,
                             &(*settings)[NM_SETTING_WIRED_SETTING_NAME]);
            }
            else if (auto* wireless = std::get_if<WirelessConnectionData>(&data.specific_data))
            {
                insert_wireless(*wireless,
                                &(*settings)[NM_SETTING_WIRELESS_SETTING_NAME]);

                if (wireless->key_mgmt_type() != KEY_EMPTY)
                {
                    insert_wireless_security(
                        *wireless,
                        &(*settings)[NM_SETTING_WIRELESS_SECURITY_SETTING_NAME]);
                }

                if (auto* eap = std::get_if<EAP_Data>(&wireless->auth))
                {
                    insert_eap(*eap,
                               &(*settings)[NM_SETTING_802_1X_SETTING_NAME]);
                }
            }

            insert_ip4config(data.ip4config,
                             &(*settings)[NM_SETTING_IP4_CONFIG_SETTING_NAME]);

            insert_ip6config(data.ip6config,
                             &(*settings)[NM_SETTING_IP6_CONFIG_SETTING_NAME]);
        }

        cc::glib::SettingsContainer build_settings_container(
            const ConnectionData& data)
        {
            // Unfortunately Glibmm does not have a good way to construct
            // VariantContainer values from Variant values, so we use C here. :(

            GVariantBuilder builder;
            g_variant_builder_init(&builder, G_VARIANT_TYPE("a{sa{sv}}"));

            cc::glib::SettingsMap map;
            insert_settings(data, &map);

            for (const auto& outer : map)
            {
                g_variant_builder_open(&builder, G_VARIANT_TYPE("{sa{sv}}"));
                g_variant_builder_add(&builder, "s", outer.first.c_str());
                g_variant_builder_open(&builder, G_VARIANT_TYPE("a{sv}"));

                for (const auto& inner : outer.second)
                {
                    // g_variant_builder_add(&builder,
                    //                       G_VARIANT_TYPE("{sv}"),
                    //                       inner.first.c_str(),
                    //                       inner.second.gobj_copy());
                    g_variant_builder_open(&builder, G_VARIANT_TYPE("{sv}"));
                    g_variant_builder_add(&builder, "s", inner.first.c_str());
                    g_variant_builder_add(&builder, "v", inner.second.gobj_copy());
                    g_variant_builder_close(&builder);
                }

                g_variant_builder_close(&builder);
                g_variant_builder_close(&builder);
            }

            Glib::VariantBase settings = Glib::wrap(g_variant_builder_end(&builder));
            return Glib::VariantBase::cast_dynamic<cc::glib::SettingsContainer>(settings);
        }

    }  // namespace connection
}  // namespace cc::platform::netconfig::dbus
