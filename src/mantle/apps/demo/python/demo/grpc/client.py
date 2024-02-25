#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_grpc_client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core import API, CC, ProtoBuf, SignalSlot, demo_signals
from cc.messaging.grpc.signalclient import SignalClient


#===============================================================================
# SignalClient class

class DemoClient (API, SignalClient):
    '''Client for Demo service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from demo_service_pb2_grpc import DemoStub as Stub

    ## `service_name` is optional. If not provided here it is determined
    ## from the above stub.  It is used to look up service specific settings,
    ## such as target host/port.
    service_name = None

    ## `signal_type` is required only if we don't provide a an existing
    ## SignalStore() instance to the `SignalClient.__init__()` base, below.
    ## In our case we do, since we share the signal store with other message
    ## clients which also receive and re-emit signals from remote endpoints.
    ## However, let's set this variable anyway for illustration.
    signal_type = CC.Demo.Signal


    def __init__(self,
                 host           : str = "",      # gRPC server
                 identity       : str = None,    # Greeter identity
                 wait_for_ready : bool = False): # Keep trying to connect

        API.__init__(self, 'Python gRPC client', identity)
        SignalClient.__init__(self,
                              host = host,
                              signal_store = demo_signals,
                              watch_all = False,
                              wait_for_ready = wait_for_ready)


    def say_hello(self, greeting: CC.Demo.Greeting):
        self.check_type(greeting, CC.Demo.Greeting)
        self._wrap(self.stub.say_hello, greeting)

    def get_current_time(self) -> CC.Demo.TimeData:
        return self._wrap(self.stub.get_current_time)

    def start_ticking(self) -> None:
        self._wrap(self.stub.start_ticking)

    def stop_ticking(self) -> None:
        self._wrap(self.stub.stop_ticking)

    def start_notify_greetings(self, callback: SignalSlot):
        API.start_notify_greetings(self, callback)
        self.start_watching()

    def start_notify_time(self, callback: SignalSlot):
        API.start_notify_time(self, callback)
        self.start_watching()
