/// -*- c++ -*-
//==============================================================================
/// @file protobuf-network.h++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network.pb.h"  // generated from `variant.proto`
#include "network-provider.h++"

namespace protobuf
{
    //==========================================================================
    // ::platform::network::GlobalData

    void encode(const ::platform::network::GlobalData &data,
                ::cc::network::GlobalData *msg) noexcept;

    void decode(const ::cc::network::GlobalData &msg,
                ::platform::network::GlobalData *data) noexcept;

    //==========================================================================
    // ::platform::network::IPConfig

    void encode(const ::platform::network::IPConfigData &config,
                ::cc::network::IPConfigData *msg) noexcept;

    void decode(const ::cc::network::IPConfigData &msg,
                ::platform::network::IPConfigData *config) noexcept;

    //==========================================================================
    // ::platform::network::IPConfigMethod

    void encode(::platform::network::IPConfigMethod method,
                ::cc::network::IPConfigMethod *protomethod) noexcept;

    void decode(::cc::network::IPConfigMethod protomethod,
                ::platform::network::IPConfigMethod *method) noexcept;

    //==========================================================================
    // ::platform::network::AddressData

    void encode(const ::platform::network::AddressData &address,
                ::cc::network::AddressData *msg) noexcept;

    void decode(const ::cc::network::AddressData &msg,
                ::platform::network::AddressData *address) noexcept;

    //==========================================================================
    // ::platform::network::ConnectionMap

    void encode(const ::platform::network::ConnectionMap &map,
                ::cc::network::ConnectionMap *msg) noexcept;

    void decode(const ::cc::network::ConnectionMap &msg,
                ::platform::network::ConnectionMap *map) noexcept;

    //==========================================================================
    // ::platform::network::ConnectionData

    void encode(const ::platform::network::ConnectionData &conn,
                ::cc::network::ConnectionData *msg) noexcept;

    void decode(const ::cc::network::ConnectionData &msg,
                ::platform::network::ConnectionData *conn) noexcept;

    //==========================================================================
    // ::platform::network::WiredConnectionData

    void encode(const ::platform::network::WiredConnectionData &data,
                ::cc::network::WiredConnectionData *msg) noexcept;

    void decode(const ::cc::network::WiredConnectionData &msg,
                ::platform::network::WiredConnectionData *data) noexcept;

    //==========================================================================
    // ::platform::network::WirelessConnectionData

    void encode(const ::platform::network::WirelessConnectionData &data,
                ::cc::network::WirelessConnectionData *msg) noexcept;

    void decode(const ::cc::network::WirelessConnectionData &msg,
                ::platform::network::WirelessConnectionData *data) noexcept;

    //==========================================================================
    // ::platform::network::WEP_Data

    void encode(const ::platform::network::WEP_Data &auth,
                ::cc::network::WEP_Data *msg) noexcept;

    void decode(const ::cc::network::WEP_Data &msg,
                ::platform::network::WEP_Data *data) noexcept;

    //==========================================================================
    // ::platform::network::WPA_Data

    void encode(const ::platform::network::WPA_Data &auth,
                ::cc::network::WPA_Data *msg) noexcept;

    void decode(const ::cc::network::WPA_Data &msg,
                ::platform::network::WPA_Data *data) noexcept;

    //==========================================================================
    // ::platform::network::EAP_Data

    void encode(const ::platform::network::EAP_Data &auth,
                ::cc::network::EAP_Data *msg) noexcept;

    void decode(const ::cc::network::EAP_Data &msg,
                ::platform::network::EAP_Data *data) noexcept;

    //==========================================================================
    // ::platform::network::ActiveConnectionMap

    void encode(const ::platform::network::ActiveConnectionMap &map,
                ::cc::network::ActiveConnectionMap *msg) noexcept;

    void decode(const ::cc::network::ActiveConnectionMap &msg,
                ::platform::network::ActiveConnectionMap *map) noexcept;

    //==========================================================================
    // ::platform::network::ActiveConnectionData

    void encode(const ::platform::network::ActiveConnectionData &conn,
                ::cc::network::ActiveConnectionData *msg) noexcept;

    void decode(const ::cc::network::ActiveConnectionData &msg,
                ::platform::network::ActiveConnectionData *conn) noexcept;

    //==========================================================================
    // ::platform::network::Devicedata

    void encode(const ::platform::network::DeviceData &device,
                ::cc::network::DeviceData *msg) noexcept;

    void decode(const ::cc::network::DeviceData &msg,
                ::platform::network::DeviceData *device) noexcept;

    //==========================================================================
    // ::platform::network::WiredDeviceData

    void encode(const ::platform::network::WiredDeviceData &data,
                ::cc::network::WiredDeviceData *msg) noexcept;

    void decode(const ::cc::network::WiredDeviceData &msg,
                ::platform::network::WiredDeviceData *data) noexcept;

    //==========================================================================
    // ::platform::network::WirelessDeviceData

    void encode(const ::platform::network::WirelessDeviceData &data,
                ::cc::network::WirelessDeviceData *msg) noexcept;

    void decode(const ::cc::network::WirelessDeviceData &msg,
                ::platform::network::WirelessDeviceData *data) noexcept;

    //==========================================================================
    // ::platform::network::DeviceMap

    void encode(const ::platform::network::DeviceMap &map,
                ::cc::network::DeviceMap *msg) noexcept;

    void decode(const ::cc::network::DeviceMap &msg,
                ::platform::network::DeviceMap *map) noexcept;

    //==========================================================================
    // ::platform::network::AccessPointMap

    void encode(const ::platform::network::AccessPointMap &map,
                ::cc::network::AccessPointMap *msg) noexcept;

    void decode(const ::cc::network::AccessPointMap &msg,
                ::platform::network::AccessPointMap *map) noexcept;

    //==========================================================================
    // ::platform::network::SSIDMap

    void encode(const ::platform::network::SSIDMap &map,
                ::cc::network::AccessPointMap *msg) noexcept;

    void decode(const ::cc::network::AccessPointMap &msg,
                ::platform::network::SSIDMap *map) noexcept;

    //==========================================================================
    // ::platform::network::AccessPointData

    void encode(const ::platform::network::AccessPointData &ap,
                ::cc::network::AccessPointData *msg) noexcept;

    void decode(const ::cc::network::AccessPointData &msg,
                ::platform::network::AccessPointData *ap) noexcept;

    //==========================================================================
    // ::platform::network::WirelessBandSetting

    void encode(::platform::network::WirelessBandSelection band_selection,
                ::cc::network::WirelessBandSelection *proto_band) noexcept;

    void decode(::cc::network::WirelessBandSelection proto_band,
                ::platform::network::WirelessBandSelection *band_selection) noexcept;

    //==========================================================================
    // NM80211Mode

    void decode(::cc::network::WirelessMode protomode, NM80211Mode *mode) noexcept;
    void encode(NM80211Mode mode, ::cc::network::WirelessMode *protomode) noexcept;

    //==========================================================================
    // ::platform::network::NM80211Mode

    void encode(NM80211Mode mode, ::cc::network::WirelessMode *protomode) noexcept;
    void decode(::cc::network::WirelessMode protomode, NM80211Mode *mode) noexcept;

    //==========================================================================
    // ::platform::network::KeyManagement

    void encode(::platform::network::KeyManagement key_mgmt,
                ::cc::network::KeyManagement *proto_key_mgmt) noexcept;

    void decode(::cc::network::KeyManagement proto_key_mgmt,
                ::platform::network::KeyManagement *key_mgmt) noexcept;

    //==========================================================================
    // ::platform::network::AuthenticationType

    void encode(::platform::network::AuthenticationType key_mgmt,
                ::cc::network::AuthenticationType *proto_key_mgmt) noexcept;

    void decode(::cc::network::AuthenticationType proto_key_mgmt,
                ::platform::network::AuthenticationType *key_mgmt) noexcept;

    //==========================================================================
    // ::platform::network::AuthenticationAlgorithm

    void encode(::platform::network::AuthenticationAlgorithm alg,
                ::cc::network::AuthenticationAlgorithm *proto_alg) noexcept;

    void decode(::cc::network::AuthenticationAlgorithm proto_alg,
                ::platform::network::AuthenticationAlgorithm *alg) noexcept;

    //==========================================================================
    // ::platform::network::EAP_Type

    void encode(::platform::network::EAP_Type eap_type,
                ::cc::network::EAP_Type *proto_eap_type) noexcept;

    void decode(::cc::network::EAP_Type proto_eap_type,
                ::platform::network::EAP_Type *eap_type) noexcept;

    //==========================================================================
    // ::platform::network::EAP_Phase2

    void encode(::platform::network::EAP_Phase2 phase2,
                ::cc::network::EAP_Phase2 *proto_phase2) noexcept;

    void decode(::cc::network::EAP_Phase2 proto_phase2,
                ::platform::network::EAP_Phase2 *phase2) noexcept;

    //==========================================================================
    // ::platform::network::FAST_Provisioning

    void encode(::platform::network::FAST_Provisioning provisioning,
                ::cc::network::FAST_Provisioning *proto_provisioning) noexcept;

    void decode(::cc::network::FAST_Provisioning proto_provisioning,
                ::platform::network::FAST_Provisioning *provisioning) noexcept;

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::network::WEP_KeyType *proto_type) noexcept;

    void decode(::cc::network::WEP_KeyType proto_type,
                NMWepKeyType *type) noexcept;

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type,
                ::cc::network::DeviceType *prototype) noexcept;

    void decode(::cc::network::DeviceType prototype,
                NMDeviceType *type) noexcept;

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::network::DeviceState *protostate) noexcept;

    void decode(::cc::network::DeviceState protostate,
                NMDeviceState *state) noexcept;

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::network::DeviceStateReason *proto) noexcept;

    void decode(::cc::network::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept;

    //==========================================================================
    // ::platform::network::ConnectionType

    void encode(::platform::network::ConnectionType type,
                ::cc::network::ConnectionType *prototype) noexcept;

    void decode(::cc::network::ConnectionType prototype,
                ::platform::network::ConnectionType *type) noexcept;

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::network::ActiveConnectionState *protostate) noexcept;

    void decode(::cc::network::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept;

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::network::ActiveConnectionStateReason *proto) noexcept;

    void decode(::cc::network::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept;

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::network::ConnectivityState *proto) noexcept;

    void decode(::cc::network::ConnectivityState proto,
                NMConnectivityState *state) noexcept;

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::network::NetworkState *proto) noexcept;

    void decode(::cc::network::NetworkState proto,
                NMState *state) noexcept;

}  // namespace protobuf
