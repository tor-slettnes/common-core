#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file netconfig.py
## @brief Utility functions for data types from `neconfig.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .netconfig_types_pb2 import *
from cc.protobuf.utils import proto_enum

ActivationStateFlags        = proto_enum(ActivationStateFlags)
ActiveConnectionState       = proto_enum(ActiveConnectionState)
ActiveConnectionStateReason = proto_enum(ActiveConnectionStateReason)
AuthenticationAlgorithm     = proto_enum(AuthenticationAlgorithm)
AuthenticationProtocol      = proto_enum(AuthenticationProtocol)
AuthenticationType          = proto_enum(AuthenticationType)
ConnectionType              = proto_enum(ConnectionType)
ConnectivityState           = proto_enum(ConnectivityState)
DeviceFlags                 = proto_enum(DeviceFlags)
DeviceState                 = proto_enum(DeviceState)
DeviceStateReason           = proto_enum(DeviceStateReason)
DeviceType                  = proto_enum(DeviceType)
EAPPhase2                   = proto_enum(EAPPhase2)
EAPType                     = proto_enum(EAPType)
FASTProvisioning            = proto_enum(FASTProvisioning)
IPConfigMethod              = proto_enum(IPConfigMethod)
KeyManagement               = proto_enum(KeyManagement)
NetworkState                = proto_enum(NetworkState)
WEPKeyType                  = proto_enum(WEPKeyType)
WirelessBandSelection       = proto_enum(WirelessBandSelection)
WirelessMode                = proto_enum(WirelessMode)
