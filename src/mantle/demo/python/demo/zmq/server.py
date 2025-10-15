#!/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief Python server for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.messaging.zmq.basic import Publisher
from cc.messaging.zmq.protobuf.server import Server as ProtoServer
from cc.messaging.zmq.protobuf.signalwriter import SignalWriter
from cc.demo.base import API, demo_signals
from cc.demo.zmq.requesthandler import DemoRequestHandler
from cc.demo.zmq.common import DEMO_SERVICE_CHANNEL, DEMO_PUBLISHER_CHANNEL

class Server (ProtoServer):
    def __init__(self,
                 api_provider: API,
                 bind_service_address: str = "",
                 bind_publisher_address: str = ""):

        ProtoServer.__init__(
            self,
            bind_address = bind_service_address,
            channel_name = DEMO_SERVICE_CHANNEL,
            request_handlers = [
                DemoRequestHandler(api_provider),
            ])

        self.publisher = Publisher(
            bind_address = bind_publisher_address,
            channel_name = DEMO_PUBLISHER_CHANNEL,
            product_name = self.product_name)

        self.signalwriter = SignalWriter(self.publisher, demo_signals)

    def initialize(self):
        ProtoServer.initialize(self)
        self.publisher.initialize()
        self.signalwriter.start()

    def deinitialize(self):
        self.signalwriter.stop()
        self.publisher.deinitialize()
        ProtoServer.deinitialize(self)


if __name__ == '__main__':
    from cc.demo.native import NativeDemo
    import logging

    pylogger = logging.getLogger()
    pylogger.setLevel(logging.INFO)

    demoserver = Server(api_provider = NativeDemo())
    logging.info("Starting Python ZMQ Demo Server")
    demoserver.run()
