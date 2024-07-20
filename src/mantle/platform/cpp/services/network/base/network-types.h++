// -*- c++ -*-
//==============================================================================
/// @file network-types.h++
/// @brief Network service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/provider.h++"
#include "types/bytevector.h++"
#include "types/streamable.h++"
#include "types/filesystem.h++"
#include "types/symbolmap.h++"

#include <NetworkManager.h>
#include <map>

#define WEP_KEY_COUNT 4

namespace platform::network
{
    using Key = std::string;
    using HWAddress = std::string;
    using IPAddress = std::string;
    using Domain = std::string;
    using SSID = core::types::ByteVector;

    enum IPConfigMethod
    {
        METHOD_NONE,
        METHOD_DISABLED,
        METHOD_AUTO,
        METHOD_DHCP,
        METHOD_MANUAL,
        METHOD_LINK_LOCAL
    };

    struct MappedData
    {
        virtual std::string key() const = 0;
    };

    struct SystemData : public core::types::Streamable
    {
        std::string hostname;

        void to_stream(std::ostream &stream) const override;
    };

    struct AddressData : public core::types::Streamable
    {
        IPAddress address;
        uint prefixlength;

        void to_stream(std::ostream &stream) const override;
    };

    using AddressVector = std::vector<AddressData>;

    struct IPConfigData : public core::types::Streamable
    {
        IPConfigMethod method = METHOD_NONE;
        AddressVector address_data;
        IPAddress gateway;
        std::vector<IPAddress> dns;
        std::vector<Domain> searches;

        void clear();
        void to_stream(std::ostream &stream) const override;
    };

    using IPConfigRef = std::shared_ptr<IPConfigData>;
    // using IPConfigMap = std::unordered_map<Key, IPConfigRef>;

    //==========================================================================
    // Wired connection data

    struct WiredConnectionData : public core::types::Streamable
    {
        bool auto_negotiate;
        void to_stream(std::ostream &stream) const override;
    };

    //==========================================================================
    // Wireless connection data

    // enum WirelessAPMode
    // {
    //     NM_802_11_MODE_UNKNOWN,
    //     AP_MODE_ADHOC,
    //     AP_MODE_INFRA
    // };

    enum WirelessBandSelection
    {
        BAND_ANY,  // Either 2.4 GHz or 5 GHz
        BAND_A,    // 5 GHz only
        BAND_BG    // 2.4 GHz only
    };

    enum KeyManagement
    {
        KEY_EMPTY,
        KEY_WEP,
        KEY_IEEE8021X,
        KEY_PSK,
        KEY_SAE,
        KEY_OWE,
        KEY_EAP
    };

    enum AuthenticationAlgorithm
    {
        AUTH_ALG_NONE,
        AUTH_ALG_WEP_OPEN,
        AUTH_ALG_WEP_SHARED,
        AUTH_ALG_LEAP
    };

    enum EAP_Type
    {
        EAP_NONE,
        EAP_MD5,
        EAP_TLS,
        EAP_TTLS,
        EAP_FAST,
        EAP_LEAP,
        EAP_PEAP,
        EAP_PWD
    };

    enum EAP_Phase2
    {
        Phase2_NONE,
        Phase2_MD5,
        Phase2_GTC,
        Phase2_PAP,
        Phase2_CHAP,
        Phase2_MSCHAP,
        Phase2_MSCHAPv2,
        Phase2_MSCHAPv2_NO_EAP
    };

    enum FAST_Provisioning
    {
        Provisioning_NONE = 0,
        Provisioning_DISABLED = 4,
        Provisioning_ANONYMOUS = 5,
        Provisioning_AUTHENTICATED = 6,
        Provisioning_ANY = 7
    };

    struct WEP_Data : public core::types::Streamable
    {
        AuthenticationAlgorithm auth_alg = AUTH_ALG_NONE;
        std::vector<core::types::ByteVector> keys = {{}, {}, {}, {}};
        uint key_idx = 0;
        NMWepKeyType key_type = NM_WEP_KEY_TYPE_KEY;

        void to_stream(std::ostream &stream) const override;
    };

    struct WPA_Data : public core::types::Streamable
    {
        std::string psk;

        void to_stream(std::ostream &stream) const override;
    };

    struct EAP_Data : public core::types::Streamable
    {
        AuthenticationAlgorithm auth_alg = AUTH_ALG_NONE;
        EAP_Type eap_type = EAP_NONE;
        EAP_Phase2 eap_phase2 = Phase2_NONE;
        std::string anonymous_identity;
        std::string domain;
        std::string identity;
        std::string password;
        std::filesystem::path ca_cert;
        std::string ca_cert_password;
        std::filesystem::path client_cert;
        std::filesystem::path client_cert_key;
        std::string client_cert_password;
        std::filesystem::path pac_file;
        FAST_Provisioning fast_provisioning = Provisioning_NONE;

        void to_stream(std::ostream &stream) const override;
    };

    enum AuthenticationType
    {
        AUTH_TYPE_NONE,
        AUTH_TYPE_WEP,
        AUTH_TYPE_WPA,
        AUTH_TYPE_EAP,
        AUTH_TYPE_UNKNOWN = 15,
    };

    enum AuthenticationProtocol
    {
        AUTH_PROTO_WPA = 0x0001,
        AUTH_PROTO_RSN = 0x0002,
    };

    using AuthenticationData =
        std::variant<std::monostate,
                     WEP_Data,
                     WPA_Data,
                     EAP_Data>;

    struct WirelessConnectionData : public core::types::Streamable
    {
        SSID ssid;
        NM80211Mode mode = NM_802_11_MODE_UNKNOWN;
        bool hidden = false;
        uint tx_power = 0;
        KeyManagement key_mgmt = KEY_EMPTY;
        uint auth_protos = 0;
        AuthenticationData auth;
        std::optional<WirelessBandSelection> band;

        KeyManagement key_mgmt_type() const;
        AuthenticationType auth_type() const;
        WEP_Data *auth_wep();
        WPA_Data *auth_wpa();
        EAP_Data *auth_eap();
        void to_stream(std::ostream &stream) const override;
    };

    //==========================================================================
    // Connection Data

    enum ConnectionType
    {
        CONN_TYPE_UNKNOWN,
        CONN_TYPE_WIRED,
        CONN_TYPE_WIRELESS
    };

    using ConnectionSpecificData =
        std::variant<std::monostate,
                     WiredConnectionData,
                     WirelessConnectionData>;

    struct ConnectionData : public core::types::Streamable, public MappedData
    {
        std::string id;
        std::string uuid;
        std::string interface;
        ConnectionSpecificData specific_data;
        IPConfigData ip4config;
        IPConfigData ip6config;

        std::string key() const override;
        ConnectionType type() const;
        WiredConnectionData *wired_data();
        WirelessConnectionData *wifi_data();
        bool is_valid() const;
        void to_stream(std::ostream &stream) const override;
    };

    using ConnectionRef = std::shared_ptr<ConnectionData>;
    using ConnectionMap = std::unordered_map<Key, ConnectionRef>;

    //==========================================================================
    // ActiveConnection Data

    struct ActiveConnectionData : public core::types::Streamable, public MappedData
    {
        std::string uuid;
        std::string id;
        ConnectionType type = CONN_TYPE_UNKNOWN;
        NMActiveConnectionState state = NM_ACTIVE_CONNECTION_STATE_UNKNOWN;
        NMActiveConnectionStateReason state_reason = NM_ACTIVE_CONNECTION_STATE_REASON_UNKNOWN;
        uint state_flags = 0;
        bool default4 = false;
        IPConfigData ip4config;
        bool default6 = false;
        IPConfigData ip6config;
        bool vpn = false;

        std::string key() const override;
        void to_stream(std::ostream &stream) const override;
        bool is_connected() const;
        bool is_busy() const;
        bool has_gateway() const;
    };

    using ActiveConnectionRef = std::shared_ptr<ActiveConnectionData>;
    using ActiveConnectionMap = std::unordered_map<Key, ActiveConnectionRef>;

    //==========================================================================
    // Access Point Data

    using FrequencyRangeMap = std::unordered_map<WirelessBandSelection, std::pair<uint, uint>>;

    struct AccessPointData : public core::types::Streamable, public MappedData
    {
        static FrequencyRangeMap frequency_ranges;

        SSID ssid;
        uint frequency = 0;
        uint flags = NM_802_11_AP_FLAGS_NONE;
        uint rsn_flags = NM_802_11_AP_SEC_NONE;
        uint wpa_flags = NM_802_11_AP_SEC_NONE;
        // AuthenticationType authType = AUTH_TYPE_NONE;
        HWAddress hwAddress;
        NM80211Mode mode = NM_802_11_MODE_UNKNOWN;
        uint maxbitrate = 0;
        core::types::Byte strength = 0;
        core::dt::TimePoint lastSeen;

        std::string key() const override;
        uint auth_flags() const;
        AuthenticationType auth_type() const;
        bool auth_required() const;
        WirelessBandSelection band() const;
        void to_stream(std::ostream &stream) const override;
    };

    // class AccessPointMap : std::map<Key, AccessPointData, AccessPointMap::stronger>
    // {
    //     constexpr bool stronger(const Key &lhs, const Key &rhs)
    //     {
    //         return (this->at(lhs).strength > this->at(rhs).strength);
    //     }
    // };

    using AccessPointRef = std::shared_ptr<AccessPointData>;
    using AccessPointMap = std::unordered_map<Key, AccessPointRef>;
    using SSIDMap = std::unordered_map<SSID, AccessPointRef>;

    //==========================================================================
    // Device Data

    struct WiredDeviceData : public core::types::Streamable
    {
        uint speed = 0;

        void to_stream(std::ostream &stream) const override;
    };

    struct WirelessDeviceData : public core::types::Streamable
    {
        NM80211Mode mode = NM_802_11_MODE_UNKNOWN;
        uint bitrate = 0;
        Key active_accesspoint;
        core::dt::TimePoint lastScan;

        void to_stream(std::ostream &stream) const override;
    };

    using DeviceSpecificData =
        std::variant<std::monostate,
                     WiredDeviceData,
                     WirelessDeviceData>;

    struct DeviceData : public core::types::Streamable, public MappedData
    {
        NMDeviceType type = NM_DEVICE_TYPE_UNKNOWN;
        NMDeviceState state = NM_DEVICE_STATE_UNKNOWN;
        NMDeviceStateReason state_reason = NM_DEVICE_STATE_REASON_NONE;
        std::uint32_t flags = 0;  //NM_DEVICE_INTERFACE_FLAG_NONE;
        std::string interface;
        std::string hwAddress;
        IPConfigData ip4config;
        IPConfigData ip6config;
        NMConnectivityState ip4connectivity = NM_CONNECTIVITY_UNKNOWN;
        NMConnectivityState ip6connectivity = NM_CONNECTIVITY_UNKNOWN;
        Key active_connection;
        DeviceSpecificData specific_data;

        std::string key() const override;
        const WiredDeviceData *wired_data() const;
        const WirelessDeviceData *wifi_data() const;
        bool is_managed() const;
        void to_stream(std::ostream &stream) const override;
    };

    using DeviceRef = std::shared_ptr<DeviceData>;
    using DeviceMap = std::unordered_map<Key, DeviceRef>;

    using WifiTuple = std::tuple<DeviceRef,
                                 AccessPointRef,
                                 ActiveConnectionRef,
                                 ConnectionRef>;

    //==========================================================================
    // Global State Data

    struct GlobalData : public core::types::Streamable
    {
        NMState state = NM_STATE_UNKNOWN;
        NMConnectivityState connectivity = NM_CONNECTIVITY_UNKNOWN;
        bool wireless_hardware_enabled = false;
        bool wireless_enabled = false;
        bool wireless_allowed = true;
        WirelessBandSelection wireless_band_selection = BAND_ANY;

        void to_stream(std::ostream &stream) const override;
    };

    using GlobalDataRef = std::shared_ptr<GlobalData>;

    /// Input stream support for enumerated types and variants
    std::istream &operator>>(std::istream &stream, WirelessBandSelection &band);

    /// Output stream support for enumerated types and variants
    std::ostream &operator<<(std::ostream &stream, WirelessBandSelection band);
    std::ostream &operator<<(std::ostream &stream, IPConfigMethod method);
    std::ostream &operator<<(std::ostream &stream, KeyManagement key_mgmt);
    std::ostream &operator<<(std::ostream &stream, AuthenticationProtocol auth_proto);
    std::ostream &operator<<(std::ostream &stream, AuthenticationAlgorithm auth_alg);
    std::ostream &operator<<(std::ostream &stream, EAP_Type eap_type);
    std::ostream &operator<<(std::ostream &stream, EAP_Phase2 eap_phase2);
    std::ostream &operator<<(std::ostream &stream, FAST_Provisioning provisioning);
    std::ostream &operator<<(std::ostream &stream, AuthenticationType auth_type);
    std::ostream &operator<<(std::ostream &stream, AuthenticationData auth_data);
    std::ostream &operator<<(std::ostream &stream, ConnectionType conn_type);
    std::ostream &operator<<(std::ostream &stream, ConnectionSpecificData conn_data);
    std::ostream &operator<<(std::ostream &stream, DeviceSpecificData dev_data);

    /// Lookup maps to from enumerated values to strings
    extern const core::types::SymbolMap<NMState> state_map;
    extern const core::types::SymbolMap<IPConfigMethod> ipconfig_method_map;
    extern const core::types::SymbolMap<WirelessBandSelection> band_selection_map;
    extern const core::types::SymbolMap<KeyManagement> key_mgmt_map;
    extern const core::types::SymbolMap<AuthenticationProtocol> auth_proto_map;
    extern const core::types::SymbolMap<AuthenticationAlgorithm> auth_alg_map;
    extern const core::types::SymbolMap<AuthenticationType> auth_type_map;
    extern const core::types::SymbolMap<EAP_Type> eap_type_map;
    extern const core::types::SymbolMap<EAP_Phase2> eap_phase2_map;
    extern const core::types::SymbolMap<FAST_Provisioning> fast_provisioning_map;
    extern const core::types::SymbolMap<ConnectionType> connection_type_map;
    extern const core::types::SymbolMap<NMWepKeyType> wep_key_type_map;
    extern const core::types::SymbolMap<NMDeviceType> device_type_map;
    extern const core::types::SymbolMap<NMDeviceState> device_state_map;
    extern const core::types::SymbolMap<NMDeviceStateReason> device_state_reasy_map;
    extern const core::types::SymbolMap<NM80211Mode> ap_mode_map;
    extern const core::types::SymbolMap<NMActiveConnectionState> ac_state_map;
    extern const core::types::SymbolMap<NMActiveConnectionStateReason> ac_reason_map;
    extern const core::types::SymbolMap<NMConnectivityState> connectivity_state_map;

}  // namespace platform::network

std::ostream &operator<<(std::ostream &stream, NMState state);
std::ostream &operator<<(std::ostream &stream, NMWepKeyType key_type);
std::ostream &operator<<(std::ostream &stream, NMDeviceType dev_type);
std::ostream &operator<<(std::ostream &stream, NMDeviceState dev_state);
std::ostream &operator<<(std::ostream &stream, NMDeviceStateReason dev_state_reasy);
std::ostream &operator<<(std::ostream &stream, NM80211Mode mode);
std::ostream &operator<<(std::ostream &stream, NMActiveConnectionState state);
std::ostream &operator<<(std::ostream &stream, NMActiveConnectionStateReason reason);
std::ostream &operator<<(std::ostream &stream, NMConnectivityState state);
