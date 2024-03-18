#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core import API, CC, ProtoBuf, SignalSlot, demo_signals
from cc.messaging.grpc import SignalClient


#===============================================================================
# SignalClient class

class DemoClient (API, SignalClient):
    '''Client for Demo service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from generated.demo_pb2_grpc import DemoStub as Stub

    ## `service_name` is optional. If not provided here it is determined
    ## from the above stub.  It is used to look up service specific settings,
    ## such as target host/port.
    service_name = None

    ## `signal_type` is required only if we don't provide a an existing
    ## SignalStore() instance to the `SignalClient.__init__()` base, below.
    ## In our case we do, since we share the signal store with other message
    ## clients which also receive and re-emit signals from remote endpoints.
    #signal_type = CC.Demo.Signal

    def __init__(self,
                 host           : str = "",      # gRPC server
                 identity       : str = None,    # Greeter identity
                 wait_for_ready : bool = False,  # Keep trying to connect
                 use_asyncio    : bool = False): # Use Python AsyncIO semantics

        API.__init__(self, 'Python gRPC client', identity)
        SignalClient.__init__(self,
                              host = host,
                              wait_for_ready = wait_for_ready,
                              use_asyncio = use_asyncio,
                              signal_store = demo_signals,
                              watch_all = False)


    def say_hello(self, greeting: CC.Demo.Greeting):
        self.check_type(greeting, CC.Demo.Greeting)
        return self.stub.say_hello(greeting)

    def get_current_time(self) -> CC.Demo.TimeData:
        return self.stub.get_current_time(ProtoBuf.Empty())

    def start_ticking(self) -> None:
        return self.stub.start_ticking(ProtoBuf.Empty())

    def stop_ticking(self) -> None:
        return self.stub.stop_ticking(ProtoBuf.Empty())

    def start_notify_greetings(self, callback: SignalSlot):
        API.start_notify_greetings(self, callback)
        return self.start_watching()

    def start_notify_time(self, callback: SignalSlot):
        API.start_notify_time(self, callback)
        return self.start_watching()
