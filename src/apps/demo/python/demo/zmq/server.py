#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_zmq_server.py
## @brief Python server for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .requesthandler import DemoRequestHandler
from .common import DEMO_SERVICE_CHANNEL, DEMO_PUBLISHER_CHANNEL
from ..core import API, demo_signals
from ipc.zmq.protobuf.server import Server as ProtoServer
from ipc.zmq.protobuf.signalpublisher import SignalPublisher

class Server (ProtoServer):
    def __init__(self,
                 api_provider: API,
                 bind_service_address: str = "",
                 bind_publisher_address: str = ""):

        ProtoServer.__init__(self,
                             bind_address = bind_service_address,
                             channel_name = DEMO_SERVICE_CHANNEL,
                             request_handlers = [
                                 DemoRequestHandler(api_provider),
                             ])
        self.publisher = SignalPublisher(
            bind_address = bind_publisher_address,
            signal_store = demo_signals,
            channel_name = DEMO_PUBLISHER_CHANNEL)

    def initialize(self):
        ProtoServer.initialize(self)
        self.publisher.initialize()

    def deinitialize(self):
        self.publisher.deinitialize()
        ProtoServer.deinitialize(self)
