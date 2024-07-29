/// -*- c++ -*-
//==============================================================================
/// @file protobuf-netconfig-types.h++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "netconfig.pb.h"  // generated from `variant.proto`
#include "netconfig.h++"

namespace protobuf
{
    //==========================================================================
    // ::platform::netconfig::GlobalData

    void encode(const ::platform::netconfig::GlobalData &data,
                ::cc::platform::netconfig::GlobalData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::GlobalData &msg,
                ::platform::netconfig::GlobalData *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::IPConfig

    void encode(const ::platform::netconfig::IPConfigData &config,
                ::cc::platform::netconfig::IPConfigData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::IPConfigData &msg,
                ::platform::netconfig::IPConfigData *config) noexcept;

    //==========================================================================
    // ::platform::netconfig::IPConfigMethod

    void encode(::platform::netconfig::IPConfigMethod method,
                ::cc::platform::netconfig::IPConfigMethod *protomethod) noexcept;

    void decode(::cc::platform::netconfig::IPConfigMethod protomethod,
                ::platform::netconfig::IPConfigMethod *method) noexcept;

    //==========================================================================
    // ::platform::netconfig::AddressData

    void encode(const ::platform::netconfig::AddressData &address,
                ::cc::platform::netconfig::AddressData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AddressData &msg,
                ::platform::netconfig::AddressData *address) noexcept;

    //==========================================================================
    // ::platform::netconfig::ConnectionMap

    void encode(const ::platform::netconfig::ConnectionMap &map,
                ::cc::platform::netconfig::ConnectionMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ConnectionMap &msg,
                ::platform::netconfig::ConnectionMap *map) noexcept;

    //==========================================================================
    // ::platform::netconfig::ConnectionData

    void encode(const ::platform::netconfig::ConnectionData &conn,
                ::cc::platform::netconfig::ConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ConnectionData &msg,
                ::platform::netconfig::ConnectionData *conn) noexcept;

    //==========================================================================
    // ::platform::netconfig::WiredConnectionData

    void encode(const ::platform::netconfig::WiredConnectionData &data,
                ::cc::platform::netconfig::WiredConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WiredConnectionData &msg,
                ::platform::netconfig::WiredConnectionData *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::WirelessConnectionData

    void encode(const ::platform::netconfig::WirelessConnectionData &data,
                ::cc::platform::netconfig::WirelessConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WirelessConnectionData &msg,
                ::platform::netconfig::WirelessConnectionData *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::WEP_Data

    void encode(const ::platform::netconfig::WEP_Data &auth,
                ::cc::platform::netconfig::WEP_Data *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WEP_Data &msg,
                ::platform::netconfig::WEP_Data *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::WPA_Data

    void encode(const ::platform::netconfig::WPA_Data &auth,
                ::cc::platform::netconfig::WPA_Data *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WPA_Data &msg,
                ::platform::netconfig::WPA_Data *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::EAP_Data

    void encode(const ::platform::netconfig::EAP_Data &auth,
                ::cc::platform::netconfig::EAP_Data *msg) noexcept;

    void decode(const ::cc::platform::netconfig::EAP_Data &msg,
                ::platform::netconfig::EAP_Data *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::ActiveConnectionMap

    void encode(const ::platform::netconfig::ActiveConnectionMap &map,
                ::cc::platform::netconfig::ActiveConnectionMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ActiveConnectionMap &msg,
                ::platform::netconfig::ActiveConnectionMap *map) noexcept;

    //==========================================================================
    // ::platform::netconfig::ActiveConnectionData

    void encode(const ::platform::netconfig::ActiveConnectionData &conn,
                ::cc::platform::netconfig::ActiveConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ActiveConnectionData &msg,
                ::platform::netconfig::ActiveConnectionData *conn) noexcept;

    //==========================================================================
    // ::platform::netconfig::Devicedata

    void encode(const ::platform::netconfig::DeviceData &device,
                ::cc::platform::netconfig::DeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::DeviceData &msg,
                ::platform::netconfig::DeviceData *device) noexcept;

    //==========================================================================
    // ::platform::netconfig::WiredDeviceData

    void encode(const ::platform::netconfig::WiredDeviceData &data,
                ::cc::platform::netconfig::WiredDeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WiredDeviceData &msg,
                ::platform::netconfig::WiredDeviceData *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::WirelessDeviceData

    void encode(const ::platform::netconfig::WirelessDeviceData &data,
                ::cc::platform::netconfig::WirelessDeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WirelessDeviceData &msg,
                ::platform::netconfig::WirelessDeviceData *data) noexcept;

    //==========================================================================
    // ::platform::netconfig::DeviceMap

    void encode(const ::platform::netconfig::DeviceMap &map,
                ::cc::platform::netconfig::DeviceMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::DeviceMap &msg,
                ::platform::netconfig::DeviceMap *map) noexcept;

    //==========================================================================
    // ::platform::netconfig::AccessPointMap

    void encode(const ::platform::netconfig::AccessPointMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::platform::netconfig::AccessPointMap *map) noexcept;

    //==========================================================================
    // ::platform::netconfig::SSIDMap

    void encode(const ::platform::netconfig::SSIDMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::platform::netconfig::SSIDMap *map) noexcept;

    //==========================================================================
    // ::platform::netconfig::AccessPointData

    void encode(const ::platform::netconfig::AccessPointData &ap,
                ::cc::platform::netconfig::AccessPointData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointData &msg,
                ::platform::netconfig::AccessPointData *ap) noexcept;

    //==========================================================================
    // ::platform::netconfig::WirelessBandSetting

    void encode(::platform::netconfig::WirelessBandSelection band_selection,
                ::cc::platform::netconfig::WirelessBandSelection *proto_band) noexcept;

    void decode(::cc::platform::netconfig::WirelessBandSelection proto_band,
                ::platform::netconfig::WirelessBandSelection *band_selection) noexcept;

    //==========================================================================
    // NM80211Mode

    void decode(::cc::platform::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;
    void encode(NM80211Mode mode, ::cc::platform::netconfig::WirelessMode *protomode) noexcept;

    //==========================================================================
    // ::platform::netconfig::NM80211Mode

    void encode(NM80211Mode mode, ::cc::platform::netconfig::WirelessMode *protomode) noexcept;
    void decode(::cc::platform::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;

    //==========================================================================
    // ::platform::netconfig::KeyManagement

    void encode(::platform::netconfig::KeyManagement key_mgmt,
                ::cc::platform::netconfig::KeyManagement *proto_key_mgmt) noexcept;

    void decode(::cc::platform::netconfig::KeyManagement proto_key_mgmt,
                ::platform::netconfig::KeyManagement *key_mgmt) noexcept;

    //==========================================================================
    // ::platform::netconfig::AuthenticationType

    void encode(::platform::netconfig::AuthenticationType key_mgmt,
                ::cc::platform::netconfig::AuthenticationType *proto_key_mgmt) noexcept;

    void decode(::cc::platform::netconfig::AuthenticationType proto_key_mgmt,
                ::platform::netconfig::AuthenticationType *key_mgmt) noexcept;

    //==========================================================================
    // ::platform::netconfig::AuthenticationAlgorithm

    void encode(::platform::netconfig::AuthenticationAlgorithm alg,
                ::cc::platform::netconfig::AuthenticationAlgorithm *proto_alg) noexcept;

    void decode(::cc::platform::netconfig::AuthenticationAlgorithm proto_alg,
                ::platform::netconfig::AuthenticationAlgorithm *alg) noexcept;

    //==========================================================================
    // ::platform::netconfig::EAP_Type

    void encode(::platform::netconfig::EAP_Type eap_type,
                ::cc::platform::netconfig::EAP_Type *proto_eap_type) noexcept;

    void decode(::cc::platform::netconfig::EAP_Type proto_eap_type,
                ::platform::netconfig::EAP_Type *eap_type) noexcept;

    //==========================================================================
    // ::platform::netconfig::EAP_Phase2

    void encode(::platform::netconfig::EAP_Phase2 phase2,
                ::cc::platform::netconfig::EAP_Phase2 *proto_phase2) noexcept;

    void decode(::cc::platform::netconfig::EAP_Phase2 proto_phase2,
                ::platform::netconfig::EAP_Phase2 *phase2) noexcept;

    //==========================================================================
    // ::platform::netconfig::FAST_Provisioning

    void encode(::platform::netconfig::FAST_Provisioning provisioning,
                ::cc::platform::netconfig::FAST_Provisioning *proto_provisioning) noexcept;

    void decode(::cc::platform::netconfig::FAST_Provisioning proto_provisioning,
                ::platform::netconfig::FAST_Provisioning *provisioning) noexcept;

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::platform::netconfig::WEP_KeyType *proto_type) noexcept;

    void decode(::cc::platform::netconfig::WEP_KeyType proto_type,
                NMWepKeyType *type) noexcept;

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type,
                ::cc::platform::netconfig::DeviceType *prototype) noexcept;

    void decode(::cc::platform::netconfig::DeviceType prototype,
                NMDeviceType *type) noexcept;

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::platform::netconfig::DeviceState *protostate) noexcept;

    void decode(::cc::platform::netconfig::DeviceState protostate,
                NMDeviceState *state) noexcept;

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::platform::netconfig::DeviceStateReason *proto) noexcept;

    void decode(::cc::platform::netconfig::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept;

    //==========================================================================
    // ::platform::netconfig::ConnectionType

    void encode(::platform::netconfig::ConnectionType type,
                ::cc::platform::netconfig::ConnectionType *prototype) noexcept;

    void decode(::cc::platform::netconfig::ConnectionType prototype,
                ::platform::netconfig::ConnectionType *type) noexcept;

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::platform::netconfig::ActiveConnectionState *protostate) noexcept;

    void decode(::cc::platform::netconfig::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept;

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::platform::netconfig::ActiveConnectionStateReason *proto) noexcept;

    void decode(::cc::platform::netconfig::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept;

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::platform::netconfig::ConnectivityState *proto) noexcept;

    void decode(::cc::platform::netconfig::ConnectivityState proto,
                NMConnectivityState *state) noexcept;

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::platform::netconfig::NetworkState *proto) noexcept;

    void decode(::cc::platform::netconfig::NetworkState proto,
                NMState *state) noexcept;

}  // namespace protobuf
