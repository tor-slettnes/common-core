#!/usr/bin/python3 -i
#===============================================================================
## @file demo_common.py
## @brief Common types and definitions for Python 'Demo' service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ipc.google.protobuf import CC as _CC, ProtoBuf
from scalar_types        import enums

class CC (_CC):
    import demo_types_pb2 as Demo

Weekdays = enums(CC.Demo.Weekday, globals())

### Identifiers for ZMQ commands & messages
DEMO_PUBLISHER_CHANNEL = 'Demo Publisher'
DEMO_SERVICE_CHANNEL   = 'Demo Service'
DEMO_RPC_INTERFACE     = 'Demo'

