#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file netconfig.py
## @brief Utility functions for data types from `neconfig.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from cc.platform.netconfig.netconfig_pb2 import *
from .utils import proto_enum

IPConfigMethod              = proto_enum(IPConfigMethod)
WirelessMode                = proto_enum(WirelessMode)
KeyManagement               = proto_enum(KeyManagement)
AuthenticationAlgorithm     = proto_enum(AuthenticationAlgorithm)
AuthenticationType          = proto_enum(AuthenticationType)
EAPType                     = proto_enum(EAPType)
EAPPhase2                   = proto_enum(EAPPhase2)
WEPKeyType                  = proto_enum(WEPKeyType)
DeviceType                  = proto_enum(DeviceType)
DeviceState                 = proto_enum(DeviceState)
DeviceFlags                 = proto_enum(DeviceFlags)
ConnectionType              = proto_enum(ConnectionType)
ActiveConnectionState       = proto_enum(ActiveConnectionState)
ActivationStateFlags        = proto_enum(ActivationStateFlags)
ActiveConnectionStateReason = proto_enum(ActiveConnectionStateReason)
FASTProvisioning            = proto_enum(FASTProvisioning)
