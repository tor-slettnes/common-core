/// -*- c++ -*-
//==============================================================================
/// @file protobuf-netconfig-types.h++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/netconfig/protobuf/netconfig_types.pb.h"  // generated from `variant.proto`
#include "netconfig.h++"

namespace cc::protobuf
{
    //==========================================================================
    // platform::netconfig::GlobalData

    void encode(const platform::netconfig::GlobalData &data,
                platform::netconfig::protobuf::GlobalData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::GlobalData &msg,
                platform::netconfig::GlobalData *data) noexcept;

    //==========================================================================
    // platform::netconfig::IPConfig

    void encode(const platform::netconfig::IPConfigData &config,
                platform::netconfig::protobuf::IPConfigData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::IPConfigData &msg,
                platform::netconfig::IPConfigData *config) noexcept;

    //==========================================================================
    // platform::netconfig::IPConfigMethod

    void encode(platform::netconfig::IPConfigMethod method,
                platform::netconfig::protobuf::IPConfigMethod *protomethod) noexcept;

    void decode(platform::netconfig::protobuf::IPConfigMethod protomethod,
                platform::netconfig::IPConfigMethod *method) noexcept;

    //==========================================================================
    // platform::netconfig::AddressData

    void encode(const platform::netconfig::AddressData &address,
                platform::netconfig::protobuf::AddressData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::AddressData &msg,
                platform::netconfig::AddressData *address) noexcept;

    //==========================================================================
    // platform::netconfig::ConnectionMap

    void encode(const platform::netconfig::ConnectionMap &map,
                platform::netconfig::protobuf::ConnectionMap *msg) noexcept;

    void decode(const platform::netconfig::protobuf::ConnectionMap &msg,
                platform::netconfig::ConnectionMap *map) noexcept;

    //==========================================================================
    // platform::netconfig::ConnectionData

    void encode(const platform::netconfig::ConnectionData &conn,
                platform::netconfig::protobuf::ConnectionData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::ConnectionData &msg,
                platform::netconfig::ConnectionData *conn) noexcept;

    //==========================================================================
    // platform::netconfig::WiredConnectionData

    void encode(const platform::netconfig::WiredConnectionData &data,
                platform::netconfig::protobuf::WiredConnectionData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WiredConnectionData &msg,
                platform::netconfig::WiredConnectionData *data) noexcept;

    //==========================================================================
    // platform::netconfig::WirelessConnectionData

    void encode(const platform::netconfig::WirelessConnectionData &data,
                platform::netconfig::protobuf::WirelessConnectionData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WirelessConnectionData &msg,
                platform::netconfig::WirelessConnectionData *data) noexcept;

    //==========================================================================
    // platform::netconfig::WEP_Data

    void encode(const platform::netconfig::WEP_Data &auth,
                platform::netconfig::protobuf::WEPData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WEPData &msg,
                platform::netconfig::WEP_Data *data) noexcept;

    //==========================================================================
    // platform::netconfig::WPA_Data

    void encode(const platform::netconfig::WPA_Data &auth,
                platform::netconfig::protobuf::WPAData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WPAData &msg,
                platform::netconfig::WPA_Data *data) noexcept;

    //==========================================================================
    // platform::netconfig::EAP_Data

    void encode(const platform::netconfig::EAP_Data &auth,
                platform::netconfig::protobuf::EAPData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::EAPData &msg,
                platform::netconfig::EAP_Data *data) noexcept;

    //==========================================================================
    // platform::netconfig::ActiveConnectionMap

    void encode(const platform::netconfig::ActiveConnectionMap &map,
                platform::netconfig::protobuf::ActiveConnectionMap *msg) noexcept;

    void decode(const platform::netconfig::protobuf::ActiveConnectionMap &msg,
                platform::netconfig::ActiveConnectionMap *map) noexcept;

    //==========================================================================
    // platform::netconfig::ActiveConnectionData

    void encode(const platform::netconfig::ActiveConnectionData &conn,
                platform::netconfig::protobuf::ActiveConnectionData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::ActiveConnectionData &msg,
                platform::netconfig::ActiveConnectionData *conn) noexcept;

    //==========================================================================
    // platform::netconfig::Devicedata

    void encode(const platform::netconfig::DeviceData &device,
                platform::netconfig::protobuf::DeviceData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::DeviceData &msg,
                platform::netconfig::DeviceData *device) noexcept;

    //==========================================================================
    // platform::netconfig::WiredDeviceData

    void encode(const platform::netconfig::WiredDeviceData &data,
                platform::netconfig::protobuf::WiredDeviceData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WiredDeviceData &msg,
                platform::netconfig::WiredDeviceData *data) noexcept;

    //==========================================================================
    // platform::netconfig::WirelessDeviceData

    void encode(const platform::netconfig::WirelessDeviceData &data,
                platform::netconfig::protobuf::WirelessDeviceData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::WirelessDeviceData &msg,
                platform::netconfig::WirelessDeviceData *data) noexcept;

    //==========================================================================
    // platform::netconfig::DeviceMap

    void encode(const platform::netconfig::DeviceMap &map,
                platform::netconfig::protobuf::DeviceMap *msg) noexcept;

    void decode(const platform::netconfig::protobuf::DeviceMap &msg,
                platform::netconfig::DeviceMap *map) noexcept;

    //==========================================================================
    // platform::netconfig::AccessPointMap

    void encode(const platform::netconfig::AccessPointMap &map,
                platform::netconfig::protobuf::AccessPointMap *msg) noexcept;

    void decode(const platform::netconfig::protobuf::AccessPointMap &msg,
                platform::netconfig::AccessPointMap *map) noexcept;

    //==========================================================================
    // platform::netconfig::SSIDMap

    void encode(const platform::netconfig::SSIDMap &map,
                platform::netconfig::protobuf::AccessPointMap *msg) noexcept;

    void decode(const platform::netconfig::protobuf::AccessPointMap &msg,
                platform::netconfig::SSIDMap *map) noexcept;

    //==========================================================================
    // platform::netconfig::AccessPointData

    void encode(const platform::netconfig::AccessPointData &ap,
                platform::netconfig::protobuf::AccessPointData *msg) noexcept;

    void decode(const platform::netconfig::protobuf::AccessPointData &msg,
                platform::netconfig::AccessPointData *ap) noexcept;

    //==========================================================================
    // platform::netconfig::WirelessBandSetting

    void encode(platform::netconfig::WirelessBandSelection band_selection,
                platform::netconfig::protobuf::WirelessBandSelection *proto_band) noexcept;

    void decode(platform::netconfig::protobuf::WirelessBandSelection proto_band,
                platform::netconfig::WirelessBandSelection *band_selection) noexcept;

    //==========================================================================
    // NM80211Mode

    void decode(platform::netconfig::protobuf::WirelessMode protomode, NM80211Mode *mode) noexcept;
    void encode(NM80211Mode mode, platform::netconfig::protobuf::WirelessMode *protomode) noexcept;

    //==========================================================================
    // platform::netconfig::NM80211Mode

    void encode(NM80211Mode mode, platform::netconfig::protobuf::WirelessMode *protomode) noexcept;
    void decode(platform::netconfig::protobuf::WirelessMode protomode, NM80211Mode *mode) noexcept;

    //==========================================================================
    // platform::netconfig::KeyManagement

    void encode(platform::netconfig::KeyManagement key_mgmt,
                platform::netconfig::protobuf::KeyManagement *proto_key_mgmt) noexcept;

    void decode(platform::netconfig::protobuf::KeyManagement proto_key_mgmt,
                platform::netconfig::KeyManagement *key_mgmt) noexcept;

    //==========================================================================
    // platform::netconfig::AuthenticationType

    void encode(platform::netconfig::AuthenticationType key_mgmt,
                platform::netconfig::protobuf::AuthenticationType *proto_key_mgmt) noexcept;

    void decode(platform::netconfig::protobuf::AuthenticationType proto_key_mgmt,
                platform::netconfig::AuthenticationType *key_mgmt) noexcept;

    //==========================================================================
    // platform::netconfig::AuthenticationAlgorithm

    void encode(platform::netconfig::AuthenticationAlgorithm alg,
                platform::netconfig::protobuf::AuthenticationAlgorithm *proto_alg) noexcept;

    void decode(platform::netconfig::protobuf::AuthenticationAlgorithm proto_alg,
                platform::netconfig::AuthenticationAlgorithm *alg) noexcept;

    //==========================================================================
    // platform::netconfig::EAP_Type

    void encode(platform::netconfig::EAP_Type eap_type,
                platform::netconfig::protobuf::EAPType *proto_eap_type) noexcept;

    void decode(platform::netconfig::protobuf::EAPType proto_eap_type,
                platform::netconfig::EAP_Type *eap_type) noexcept;

    //==========================================================================
    // platform::netconfig::EAP_Phase2

    void encode(platform::netconfig::EAP_Phase2 phase2,
                platform::netconfig::protobuf::EAPPhase2 *proto_phase2) noexcept;

    void decode(platform::netconfig::protobuf::EAPPhase2 proto_phase2,
                platform::netconfig::EAP_Phase2 *phase2) noexcept;

    //==========================================================================
    // platform::netconfig::FAST_Provisioning

    void encode(platform::netconfig::FAST_Provisioning provisioning,
                platform::netconfig::protobuf::FASTProvisioning *proto_provisioning) noexcept;

    void decode(platform::netconfig::protobuf::FASTProvisioning proto_provisioning,
                platform::netconfig::FAST_Provisioning *provisioning) noexcept;

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                platform::netconfig::protobuf::WEPKeyType *proto_type) noexcept;

    void decode(platform::netconfig::protobuf::WEPKeyType proto_type,
                NMWepKeyType *type) noexcept;

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type,
                platform::netconfig::protobuf::DeviceType *prototype) noexcept;

    void decode(platform::netconfig::protobuf::DeviceType prototype,
                NMDeviceType *type) noexcept;

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                platform::netconfig::protobuf::DeviceState *protostate) noexcept;

    void decode(platform::netconfig::protobuf::DeviceState protostate,
                NMDeviceState *state) noexcept;

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                platform::netconfig::protobuf::DeviceStateReason *proto) noexcept;

    void decode(platform::netconfig::protobuf::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept;

    //==========================================================================
    // platform::netconfig::ConnectionType

    void encode(platform::netconfig::ConnectionType type,
                platform::netconfig::protobuf::ConnectionType *prototype) noexcept;

    void decode(platform::netconfig::protobuf::ConnectionType prototype,
                platform::netconfig::ConnectionType *type) noexcept;

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                platform::netconfig::protobuf::ActiveConnectionState *protostate) noexcept;

    void decode(platform::netconfig::protobuf::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept;

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                platform::netconfig::protobuf::ActiveConnectionStateReason *proto) noexcept;

    void decode(platform::netconfig::protobuf::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept;

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                platform::netconfig::protobuf::ConnectivityState *proto) noexcept;

    void decode(platform::netconfig::protobuf::ConnectivityState proto,
                NMConnectivityState *state) noexcept;

    //==========================================================================
    // NMState

    void encode(NMState state,
                platform::netconfig::protobuf::NetworkState *proto) noexcept;

    void decode(platform::netconfig::protobuf::NetworkState proto,
                NMState *state) noexcept;

}  // namespace cc::protobuf
