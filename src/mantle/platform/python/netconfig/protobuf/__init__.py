#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file netconfig.py
## @brief Utility functions for data types from `neconfig.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .netconfig_types_pb2 import *
from cc.protobuf.utils import native_enum_from_proto

ActivationStateFlags        = native_enum_from_proto(ActivationStateFlags)
ActiveConnectionState       = native_enum_from_proto(ActiveConnectionState)
ActiveConnectionStateReason = native_enum_from_proto(ActiveConnectionStateReason)
AuthenticationAlgorithm     = native_enum_from_proto(AuthenticationAlgorithm)
AuthenticationProtocol      = native_enum_from_proto(AuthenticationProtocol)
AuthenticationType          = native_enum_from_proto(AuthenticationType)
ConnectionType              = native_enum_from_proto(ConnectionType)
ConnectivityState           = native_enum_from_proto(ConnectivityState)
DeviceFlags                 = native_enum_from_proto(DeviceFlags)
DeviceState                 = native_enum_from_proto(DeviceState)
DeviceStateReason           = native_enum_from_proto(DeviceStateReason)
DeviceType                  = native_enum_from_proto(DeviceType)
EAPPhase2                   = native_enum_from_proto(EAPPhase2)
EAPType                     = native_enum_from_proto(EAPType)
FASTProvisioning            = native_enum_from_proto(FASTProvisioning)
IPConfigMethod              = native_enum_from_proto(IPConfigMethod)
KeyManagement               = native_enum_from_proto(KeyManagement)
NetworkState                = native_enum_from_proto(NetworkState)
WEPKeyType                  = native_enum_from_proto(WEPKeyType)
WirelessBandSelection       = native_enum_from_proto(WirelessBandSelection)
WirelessMode                = native_enum_from_proto(WirelessMode)
