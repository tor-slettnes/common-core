/// -*- c++ -*-
//==============================================================================
/// @file protobuf-netconfig-types.h++
/// @brief Encode/decode routines for network ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/netconfig/netconfig.pb.h"  // generated from `variant.proto`
#include "netconfig.h++"

namespace protobuf
{
    //==========================================================================
    // ::netconfig::GlobalData

    void encode(const ::netconfig::GlobalData &data,
                ::cc::platform::netconfig::GlobalData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::GlobalData &msg,
                ::netconfig::GlobalData *data) noexcept;

    //==========================================================================
    // ::netconfig::IPConfig

    void encode(const ::netconfig::IPConfigData &config,
                ::cc::platform::netconfig::IPConfigData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::IPConfigData &msg,
                ::netconfig::IPConfigData *config) noexcept;

    //==========================================================================
    // ::netconfig::IPConfigMethod

    void encode(::netconfig::IPConfigMethod method,
                ::cc::platform::netconfig::IPConfigMethod *protomethod) noexcept;

    void decode(::cc::platform::netconfig::IPConfigMethod protomethod,
                ::netconfig::IPConfigMethod *method) noexcept;

    //==========================================================================
    // ::netconfig::AddressData

    void encode(const ::netconfig::AddressData &address,
                ::cc::platform::netconfig::AddressData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AddressData &msg,
                ::netconfig::AddressData *address) noexcept;

    //==========================================================================
    // ::netconfig::ConnectionMap

    void encode(const ::netconfig::ConnectionMap &map,
                ::cc::platform::netconfig::ConnectionMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ConnectionMap &msg,
                ::netconfig::ConnectionMap *map) noexcept;

    //==========================================================================
    // ::netconfig::ConnectionData

    void encode(const ::netconfig::ConnectionData &conn,
                ::cc::platform::netconfig::ConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ConnectionData &msg,
                ::netconfig::ConnectionData *conn) noexcept;

    //==========================================================================
    // ::netconfig::WiredConnectionData

    void encode(const ::netconfig::WiredConnectionData &data,
                ::cc::platform::netconfig::WiredConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WiredConnectionData &msg,
                ::netconfig::WiredConnectionData *data) noexcept;

    //==========================================================================
    // ::netconfig::WirelessConnectionData

    void encode(const ::netconfig::WirelessConnectionData &data,
                ::cc::platform::netconfig::WirelessConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WirelessConnectionData &msg,
                ::netconfig::WirelessConnectionData *data) noexcept;

    //==========================================================================
    // ::netconfig::WEP_Data

    void encode(const ::netconfig::WEP_Data &auth,
                ::cc::platform::netconfig::WEPData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WEPData &msg,
                ::netconfig::WEP_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::WPA_Data

    void encode(const ::netconfig::WPA_Data &auth,
                ::cc::platform::netconfig::WPAData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WPAData &msg,
                ::netconfig::WPA_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Data

    void encode(const ::netconfig::EAP_Data &auth,
                ::cc::platform::netconfig::EAPData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::EAPData &msg,
                ::netconfig::EAP_Data *data) noexcept;

    //==========================================================================
    // ::netconfig::ActiveConnectionMap

    void encode(const ::netconfig::ActiveConnectionMap &map,
                ::cc::platform::netconfig::ActiveConnectionMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ActiveConnectionMap &msg,
                ::netconfig::ActiveConnectionMap *map) noexcept;

    //==========================================================================
    // ::netconfig::ActiveConnectionData

    void encode(const ::netconfig::ActiveConnectionData &conn,
                ::cc::platform::netconfig::ActiveConnectionData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::ActiveConnectionData &msg,
                ::netconfig::ActiveConnectionData *conn) noexcept;

    //==========================================================================
    // ::netconfig::Devicedata

    void encode(const ::netconfig::DeviceData &device,
                ::cc::platform::netconfig::DeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::DeviceData &msg,
                ::netconfig::DeviceData *device) noexcept;

    //==========================================================================
    // ::netconfig::WiredDeviceData

    void encode(const ::netconfig::WiredDeviceData &data,
                ::cc::platform::netconfig::WiredDeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WiredDeviceData &msg,
                ::netconfig::WiredDeviceData *data) noexcept;

    //==========================================================================
    // ::netconfig::WirelessDeviceData

    void encode(const ::netconfig::WirelessDeviceData &data,
                ::cc::platform::netconfig::WirelessDeviceData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::WirelessDeviceData &msg,
                ::netconfig::WirelessDeviceData *data) noexcept;

    //==========================================================================
    // ::netconfig::DeviceMap

    void encode(const ::netconfig::DeviceMap &map,
                ::cc::platform::netconfig::DeviceMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::DeviceMap &msg,
                ::netconfig::DeviceMap *map) noexcept;

    //==========================================================================
    // ::netconfig::AccessPointMap

    void encode(const ::netconfig::AccessPointMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::netconfig::AccessPointMap *map) noexcept;

    //==========================================================================
    // ::netconfig::SSIDMap

    void encode(const ::netconfig::SSIDMap &map,
                ::cc::platform::netconfig::AccessPointMap *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointMap &msg,
                ::netconfig::SSIDMap *map) noexcept;

    //==========================================================================
    // ::netconfig::AccessPointData

    void encode(const ::netconfig::AccessPointData &ap,
                ::cc::platform::netconfig::AccessPointData *msg) noexcept;

    void decode(const ::cc::platform::netconfig::AccessPointData &msg,
                ::netconfig::AccessPointData *ap) noexcept;

    //==========================================================================
    // ::netconfig::WirelessBandSetting

    void encode(::netconfig::WirelessBandSelection band_selection,
                ::cc::platform::netconfig::WirelessBandSelection *proto_band) noexcept;

    void decode(::cc::platform::netconfig::WirelessBandSelection proto_band,
                ::netconfig::WirelessBandSelection *band_selection) noexcept;

    //==========================================================================
    // NM80211Mode

    void decode(::cc::platform::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;
    void encode(NM80211Mode mode, ::cc::platform::netconfig::WirelessMode *protomode) noexcept;

    //==========================================================================
    // ::netconfig::NM80211Mode

    void encode(NM80211Mode mode, ::cc::platform::netconfig::WirelessMode *protomode) noexcept;
    void decode(::cc::platform::netconfig::WirelessMode protomode, NM80211Mode *mode) noexcept;

    //==========================================================================
    // ::netconfig::KeyManagement

    void encode(::netconfig::KeyManagement key_mgmt,
                ::cc::platform::netconfig::KeyManagement *proto_key_mgmt) noexcept;

    void decode(::cc::platform::netconfig::KeyManagement proto_key_mgmt,
                ::netconfig::KeyManagement *key_mgmt) noexcept;

    //==========================================================================
    // ::netconfig::AuthenticationType

    void encode(::netconfig::AuthenticationType key_mgmt,
                ::cc::platform::netconfig::AuthenticationType *proto_key_mgmt) noexcept;

    void decode(::cc::platform::netconfig::AuthenticationType proto_key_mgmt,
                ::netconfig::AuthenticationType *key_mgmt) noexcept;

    //==========================================================================
    // ::netconfig::AuthenticationAlgorithm

    void encode(::netconfig::AuthenticationAlgorithm alg,
                ::cc::platform::netconfig::AuthenticationAlgorithm *proto_alg) noexcept;

    void decode(::cc::platform::netconfig::AuthenticationAlgorithm proto_alg,
                ::netconfig::AuthenticationAlgorithm *alg) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Type

    void encode(::netconfig::EAP_Type eap_type,
                ::cc::platform::netconfig::EAPType *proto_eap_type) noexcept;

    void decode(::cc::platform::netconfig::EAPType proto_eap_type,
                ::netconfig::EAP_Type *eap_type) noexcept;

    //==========================================================================
    // ::netconfig::EAP_Phase2

    void encode(::netconfig::EAP_Phase2 phase2,
                ::cc::platform::netconfig::EAPPhase2 *proto_phase2) noexcept;

    void decode(::cc::platform::netconfig::EAPPhase2 proto_phase2,
                ::netconfig::EAP_Phase2 *phase2) noexcept;

    //==========================================================================
    // ::netconfig::FAST_Provisioning

    void encode(::netconfig::FAST_Provisioning provisioning,
                ::cc::platform::netconfig::FASTProvisioning *proto_provisioning) noexcept;

    void decode(::cc::platform::netconfig::FASTProvisioning proto_provisioning,
                ::netconfig::FAST_Provisioning *provisioning) noexcept;

    //==========================================================================
    // NMWepKeyType

    void encode(NMWepKeyType type,
                ::cc::platform::netconfig::WEPKeyType *proto_type) noexcept;

    void decode(::cc::platform::netconfig::WEPKeyType proto_type,
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
    // ::netconfig::ConnectionType

    void encode(::netconfig::ConnectionType type,
                ::cc::platform::netconfig::ConnectionType *prototype) noexcept;

    void decode(::cc::platform::netconfig::ConnectionType prototype,
                ::netconfig::ConnectionType *type) noexcept;

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
