#!/bin/echo Do not invoke directly.
#===============================================================================
## @file logger.py
## @brief Support for ProtoBuf types from `logger.proto`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Generated from `logger.proto`
from cc.protobuf.utils import proto_enum
from cc.generated.logger_pb2 import SinkID, SinkSpecs, SinkSpec, SinkType, \
    ListenerSpec

SinkType = proto_enum(SinkType)
