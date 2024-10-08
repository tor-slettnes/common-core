#!/bin/echo Do not invoke directly.
#===============================================================================
## @file server.py
## @brief Python server for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from cc.demo.zmq.requesthandler import DemoRequestHandler
from cc.cemo.zmq.common import DEMO_SERVICE_CHANNEL, DEMO_PUBLISHER_CHANNEL
from cc.demo.core import API, signal_store
from messaging.zmq.basic import Publisher
from messaging.zmq.protobuf import Server as ProtoServer, SignalWriter

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

        self.publisher = Publisher(bind_address = bind_publisher_address,
                                   channel_name = DEMO_PUBLISHER_CHANNEL)

        self.signalwriter = SignalWriter(self.publisher, signal_store)

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

    logger = logging.getLogger()
    logger.setLevel(logging.INFO)

    demoserver = Server(api_provider = NativeDemo())
    logging.info("Starting Python ZMQ Demo Server")
    demoserver.initialize()
