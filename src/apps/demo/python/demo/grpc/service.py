#!/bin/echo Do not invoke directly.
#===============================================================================
## @file service.py
## @brief Handle `Demo` service requests
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ipc.grpc.signalservice import SignalService
from generated.demo_service_pb2_grpc import DemoServicer
from ..core import API, CC, ProtoBuf, demo_signals

### Third-party modules
import grpc

### Standard Python modules
import time


## We derive our service class from
##
##  - `Service`, the common service base, e.g. to load service settings.
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

class DemoService (SignalService, DemoServicer):
    '''Handle "Demo" service requests'''

    def __init__ (self,
                  demo_provider : API,
                  bind_address : str = ""):
        SignalService.__init__(self, demo_signals, bind_address)
        self.demo_provider = demo_provider

    def say_hello(self,
                  request: CC.Demo.Greeting,
                  context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.say_hello, request)

    def get_current_time(self,
                         request: ProtoBuf.Empty,
                         context: grpc.ServicerContext) -> CC.Demo.TimeData:
        return self._wrap(self.demo_provider.get_current_time)

    def start_ticking(self,
                      request: ProtoBuf.Empty,
                      context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.start_ticking)

    def stop_ticking(self,
                     request: ProtoBuf.Empty,
                     context: grpc.ServicerContext):
        return self._wrap(self.demo_provider.stop_ticking)
