#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..base import API, SignalSlot, signal_store
from cc.messaging.grpc import SignalClient
from cc.protobuf.wellknown import empty
from cc.protobuf.utils import check_message_type
from cc.protobuf.demo import Signal, Greeting, TimeData

#===============================================================================
# SignalClient class

class DemoClient (API, SignalClient):
    '''
    Client for Demo service.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from cc.generated.demo_pb2_grpc import DemoStub as Stub

    ## `service_name` is optional. If not provided here it is determined
    ## from the above stub.  It is used to look up service specific settings,
    ## such as target host/port.
    service_name = None

    ## `signal_type` is required only if we don't provide a an existing
    ## SignalStore() instance to the `SignalClient.__init__()` base, below.
    ## In our case we do, since we share the signal store with other message
    ## clients which also receive and re-emit signals from remote endpoints.
    #signal_type = Signal

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
                              signal_store = signal_store,
                              watch_all = False)


    def say_hello(self, greeting: Greeting):
        '''
        Send a greeting to the server.

        @param greeting:
            A greeting object.

        See also `hello()` if you want to pass in just the text of the greeting
        rather than constructing your own `Greeting` object.
        '''

        check_message_type(greeting, Greeting)
        self.stub.say_hello(greeting)

    def get_current_time(self) -> TimeData:
        '''
        Ask for the current time (local, utc, epoch) from the server.
        '''
        return self.stub.get_current_time(empty)

    def start_ticking(self) -> None:
        '''
        Tell the server to start emitting `signal_time` once per second.
        '''
        return self.stub.start_ticking(empty)

    def stop_ticking(self) -> None:
        '''
        Tell the server to stop emitting `singal_time`.
        '''
        return self.stub.stop_ticking(empty)

    def start_notify_greetings(self, callback: SignalSlot):
        '''
        Register a callback function to receive Greeting notifications from the server.
        '''
        API.start_notify_greetings(self, callback)
        return self.start_watching()

    def start_notify_time(self, callback: SignalSlot):
        '''
        Register a callback function to receive TimeData notifications from the server.
        '''
        API.start_notify_time(self, callback)
        return self.start_watching()
