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
    // ::netconfig::GlobalData

    void encode(const ::netconfig::GlobalData &data,
                ::cc::netconfig::GlobalData *msg) noexcept;

    void decode(const ::cc::netconfig::GlobalData &msg,
                ::netconfig::GlobalData *data) noexcept;

    //==========================================================================
    // ::netconfig::IPConfig

    void encode(const ::netconfig::IPConfigData &config,
                ::cc::netconfig::IPConfigData *msg) noexcept;

    void decode(const ::cc::netconfig::IPConfigData &msg,
                ::netconfig::IPConfigData *config) noexcept;

    //==========================================================================
    // ::netconfig::IPConfigMethod

    void encode(::netconfig::IPConfigMethod method,
                ::cc::netconfig::IPConfigMethod *protomethod) noexcept;

    void decode(::cc::netconfig::IPConfigMethod protomethod,
                ::netconfig::IPConfigMethod *method) noexcept;

    //==========================================================================
    // ::netconfig::AddressData

    void encode(const ::netconfig::AddressData &address,
                ::cc::netconfig::AddressData *msg) noexcept;

    void decode(const ::cc::netconfig::AddressData &msg,
                ::netconfig::AddressData *address) noexcept;

    //==========================================================================
    // ::netconfig::ConnectionMap

    void encode(const ::netconfig::ConnectionMap &map,
                ::cc::netconfig::ConnectionMap *msg) noexcept;

    void decode(const ::cc::netconfig::ConnectionMap &msg,
                ::netconfig::ConnectionMap *map) noexcept;

    //==========================================================================
    // ::netconfig::ConnectionData

    void encode(const ::netconfig::ConnectionData &conn,
                ::cc::netconfig::ConnectionData *msg) noexcept;

    void decode(const ::cc::netconfig::ConnectionData &msg,
                ::netconfig::ConnectionData *conn) noexcept;

    //==========================================================================
    // ::netconfig::WiredConnectionData

    void encode(const ::netconfig::WiredConnectionData &data,
                ::cc::netconfig::WiredConnectionData *msg) noexcept;

    void decode(const ::cc::netconfig::WiredConnectionData &msg,
                ::netconfig::WiredConnectionData *data) noexcept;

    //==========================================================================
    // ::netconfig::WirelessConnectionData

    void encode(const ::netconfig::WirelessConnectionData &data,
                ::cc::netconfig::WirelessConnectionData *msg) noexcept;

    void decode(const ::cc::netconfig::WirelessConnectionData &msg,
                ::netconfig::WirelessConnectionData *data) noexcept;

    //==========================================================================
    // ::netconfig::WEP_Data

    void encode(const ::netconfig::WEP_Data &auth,
                ::cc::netconfig::WEP_Data *msg) noexcept;

    void decode(const ::cc::netconfig::WEP_Data &msg,
                ::netconfig::WEP_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::WPA_Data

    void encode(const ::netconfig::WPA_Data &auth,
                ::cc::netconfig::WPA_Data *msg) noexcept;

    void decode(const ::cc::netconfig::WPA_Data &msg,
                ::netconfig::WPA_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Data

    void encode(const ::netconfig::EAP_Data &auth,
                ::cc::netconfig::EAP_Data *msg) noexcept;

    void decode(const ::cc::netconfig::EAP_Data &msg,
                ::netconfig::EAP_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::ActiveConnectionMap

    void encode(const ::netconfig::ActiveConnectionMap &map,
                ::cc::netconfig::ActiveConnectionMap *msg) noexcept;

    void decode(const ::cc::netconfig::ActiveConnectionMap &msg,
                ::netconfig::ActiveConnectionMap *map) noexcept;

    //==========================================================================
    // ::netconfig::ActiveConnectionData

    void encode(const ::netconfig::ActiveConnectionData &conn,
                ::cc::netconfig::ActiveConnectionData *msg) noexcept;

    void decode(const ::cc::netconfig::ActiveConnectionData &msg,
                ::netconfig::ActiveConnectionData *conn) noexcept;

    //==========================================================================
    // ::netconfig::Devicedata

    void encode(const ::netconfig::DeviceData &device,
                ::cc::netconfig::DeviceData *msg) noexcept;

    void decode(const ::cc::netconfig::DeviceData &msg,
                ::netconfig::DeviceData *device) noexcept;

    //==========================================================================
    // ::netconfig::WiredDeviceData

    void encode(const ::netconfig::WiredDeviceData &data,
                ::cc::netconfig::WiredDeviceData *msg) noexcept;

    void decode(const ::cc::netconfig::WiredDeviceData &msg,
                ::netconfig::WiredDeviceData *data) noexcept;

    //==========================================================================
    // ::netconfig::WirelessDeviceData

    void encode(const ::netconfig::WirelessDeviceData &data,
                ::cc::netconfig::WirelessDeviceData *msg) noexcept;

    void decode(const ::cc::netconfig::WirelessDeviceData &msg,
                ::netconfig::WirelessDeviceData *data) noexcept;

    //==========================================================================
    // ::netconfig::DeviceMap

    void encode(const ::netconfig::DeviceMap &map,
                ::cc::netconfig::DeviceMap *msg) noexcept;

    void decode(const ::cc::netconfig::DeviceMap &msg,
                ::netconfig::DeviceMap *map) noexcept;

    //==========================================================================
    // ::netconfig::AccessPointMap

    void encode(const ::netconfig::AccessPointMap &map,
                ::cc::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::netconfig::AccessPointMap &msg,
                ::netconfig::AccessPointMap *map) noexcept;

    //==========================================================================
    // ::netconfig::SSIDMap

    void encode(const ::netconfig::SSIDMap &map,
                ::cc::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::netconfig::AccessPointMap &msg,
                ::netconfig::SSIDMap *map) noexcept;

    //==========================================================================
    // ::netconfig::AccessPointData

    void encode(const ::netconfig::AccessPointData &ap,
                ::cc::netconfig::AccessPointData *msg) noexcept;

    void decode(const ::cc::netconfig::AccessPointData &msg,
                ::netconfig::AccessPointData *ap) noexcept;

    //==========================================================================
    // ::netconfig::WirelessBandSetting

    void encode(::netconfig::WirelessBandSelection band_selection,
                ::cc::netconfig::WirelessBandSelection *proto_band) noexcept;

    void decode(::cc::netconfig::WirelessBandSelection proto_band,
                ::netconfig::WirelessBandSelection *band_selection) noexcept;

    //==========================================================================
    // NM80211Mode

    void decode(::cc::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;
    void encode(NM80211Mode mode, ::cc::netconfig::WirelessMode *protomode) noexcept;

    //==========================================================================
    // ::netconfig::NM80211Mode

    void encode(NM80211Mode mode, ::cc::netconfig::WirelessMode *protomode) noexcept;
    void decode(::cc::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;

    //==========================================================================
    // ::netconfig::KeyManagement

    void encode(::netconfig::KeyManagement key_mgmt,
                ::cc::netconfig::KeyManagement *proto_key_mgmt) noexcept;

    void decode(::cc::netconfig::KeyManagement proto_key_mgmt,
                ::netconfig::KeyManagement *key_mgmt) noexcept;

    //==========================================================================
    // ::netconfig::AuthenticationType

    void encode(::netconfig::AuthenticationType key_mgmt,
                ::cc::netconfig::AuthenticationType *proto_key_mgmt) noexcept;

    void decode(::cc::netconfig::AuthenticationType proto_key_mgmt,
                ::netconfig::AuthenticationType *key_mgmt) noexcept;

    //==========================================================================
    // ::netconfig::AuthenticationAlgorithm

    void encode(::netconfig::AuthenticationAlgorithm alg,
                ::cc::netconfig::AuthenticationAlgorithm *proto_alg) noexcept;

    void decode(::cc::netconfig::AuthenticationAlgorithm proto_alg,
                ::netconfig::AuthenticationAlgorithm *alg) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Type

    void encode(::netconfig::EAP_Type eap_type,
                ::cc::netconfig::EAP_Type *proto_eap_type) noexcept;

    void decode(::cc::netconfig::EAP_Type proto_eap_type,
                ::netconfig::EAP_Type *eap_type) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Phase2

    void encode(::netconfig::EAP_Phase2 phase2,
                ::cc::netconfig::EAP_Phase2 *proto_phase2) noexcept;

    void decode(::cc::netconfig::EAP_Phase2 proto_phase2,
                ::netconfig::EAP_Phase2 *phase2) noexcept;

    //==========================================================================
    // ::netconfig::FAST_Provisioning

    void encode(::netconfig::FAST_Provisioning provisioning,
                ::cc::netconfig::FAST_Provisioning *proto_provisioning) noexcept;

    void decode(::cc::netconfig::FAST_Provisioning proto_provisioning,
                ::netconfig::FAST_Provisioning *provisioning) noexcept;

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::netconfig::WEP_KeyType *proto_type) noexcept;

    void decode(::cc::netconfig::WEP_KeyType proto_type,
                NMWepKeyType *type) noexcept;

    //==========================================================================
    // NMDeviceType

    void encode(NMDeviceType type,
                ::cc::netconfig::DeviceType *prototype) noexcept;

    void decode(::cc::netconfig::DeviceType prototype,
                NMDeviceType *type) noexcept;

    //==========================================================================
    // NMDeviceState

    void encode(NMDeviceState state,
                ::cc::netconfig::DeviceState *protostate) noexcept;

    void decode(::cc::netconfig::DeviceState protostate,
                NMDeviceState *state) noexcept;

    //==========================================================================
    // NMDeviceReason

    void encode(NMDeviceStateReason reason,
                ::cc::netconfig::DeviceStateReason *proto) noexcept;

    void decode(::cc::netconfig::DeviceStateReason proto,
                NMDeviceStateReason *reason) noexcept;

    //==========================================================================
    // ::netconfig::ConnectionType

    void encode(::netconfig::ConnectionType type,
                ::cc::netconfig::ConnectionType *prototype) noexcept;

    void decode(::cc::netconfig::ConnectionType prototype,
                ::netconfig::ConnectionType *type) noexcept;

    //==========================================================================
    // NMActiveConnectionState

    void encode(NMActiveConnectionState state,
                ::cc::netconfig::ActiveConnectionState *protostate) noexcept;

    void decode(::cc::netconfig::ActiveConnectionState protostate,
                NMActiveConnectionState *state) noexcept;

    //==========================================================================
    // NMActiveConnectionStateReason

    void encode(NMActiveConnectionStateReason reason,
                ::cc::netconfig::ActiveConnectionStateReason *proto) noexcept;

    void decode(::cc::netconfig::ActiveConnectionStateReason proto,
                NMActiveConnectionStateReason *reason) noexcept;

    //==========================================================================
    // NMConnectivityState

    void encode(NMConnectivityState state,
                ::cc::netconfig::ConnectivityState *proto) noexcept;

    void decode(::cc::netconfig::ConnectivityState proto,
                NMConnectivityState *state) noexcept;

    //==========================================================================
    // NMState

    void encode(NMState state,
                ::cc::netconfig::NetworkState *proto) noexcept;

    void decode(::cc::netconfig::NetworkState proto,
                NMState *state) noexcept;

}  // namespace protobuf
