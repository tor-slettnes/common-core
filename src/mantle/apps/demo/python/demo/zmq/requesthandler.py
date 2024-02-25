#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_zmq_requesthandler.py
## @brief Python request handler for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .common import DEMO_RPC_INTERFACE
from ..core import API, CC, ProtoBuf, Weekdays
from cc.messaging.zmq.protobuf.requesthandler import RequestHandler

### Standard Python modules
import logging

class DemoRequestHandler (RequestHandler):

    def __init__(self,
                 demo_provider : API):
        RequestHandler.__init__(self, DEMO_RPC_INTERFACE)
        self.demo_provider = demo_provider

    def say_hello(self, request: CC.Demo.Greeting):
        self.demo_provider.say_hello(request)

    def get_current_time(self) -> CC.Demo.TimeData:
        return self.demo_provider.get_current_time()

    def start_ticking(self):
        self.demo_provider.start_ticking()

    def stop_ticking(self):
        self.demo_provider.stop_ticking()
