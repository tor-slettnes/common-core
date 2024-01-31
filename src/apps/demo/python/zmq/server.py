#!/usr/bin/python3 -i
#===============================================================================
## @file demo_zmq_server.py
## @brief Python server for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .requesthandler import DemoRequestHandler
from ..common import CC, ProtoBuf, DEMO_SERVICE_CHANNEL
from ipc.zmq.protobuf.server import Server as ProtoServer

class Server (ProtoServer):
    def __init__(self, bind_address: str = ""):
        ProtoServer.__init__(self,
                             bind_address = bind_address,
                             channel_name = DEMO_SERVICE_CHANNEL,
                             request_handlers = [
                                 DemoRequestHandler(),
                             ])
