#!/bin/echo Do not invoke directly.
#===============================================================================
## @file request_handler.py
## @brief Handle `Demo` service requests
## @author Tor Slettnes
#===============================================================================

### Modules within current package
from cc.messaging.grpc import SignalRequestHandler
from ..base import API, demo_signals
from ..protobuf import Greeting, TimeData
from .demo_service_pb2_grpc import DemoServicer

### Third-party modules
from google.protobuf.empty_pb2 import Empty
import grpc

### Standard Python modules
import time


## We derive our service class from
##
##  - `SignalRequestHandler`, the common service base, to load service settings
##    and to propagate signals.
##
##  - `DemoServicer`, generated from the `service Demo` block in `demo_service.proto`.
##    Strictly speaking it would not be necessary to derive from this class since
##    Python does not have a concept of pure virtual methods and overrides, but
##      (a) it helps the `Service` base class determine the service name, which is
##          used e.g., to look up settings such as listening port,
##      (b) it ensures all service methods listed in the `.proto` file are
##          defined, even if not overridden here, so that the server can start, and
##      (c) doing so is in accordance with the official guide at https://grpc.io/,
##          and as such more future proof.

class DemoRequestHandler (SignalRequestHandler, DemoServicer):
    '''
    Handle "Demo" service requests
    '''

    def __init__ (self,
                  demo_provider : API,
                  bind_address : str = ""):
        SignalRequestHandler.__init__(self,
                                      signal_store = demo_signals,
                                      bind_address = bind_address)
        self.demo_provider = demo_provider

    def SayHello(self,
                 request: Greeting,
                 context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.say_hello, request)

    def GetCurrentTime(self,
                       request: Empty,
                       context: grpc.ServicerContext) -> TimeData:
        return self._wrap(self.demo_provider.get_current_time)

    def StartTicking(self,
                     request: Empty,
                     context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.start_ticking)

    def StopTicking(self,
                    request: Empty,
                    context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.stop_ticking)
