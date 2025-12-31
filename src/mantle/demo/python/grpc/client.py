#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes
#===============================================================================

### Modules within package
from ..protobuf import Signal, Greeting, TimeData
from ..base import API, SignalSlot, demo_signals

from cc.messaging.grpc import SignalClient
from cc.protobuf.wellknown import empty
from cc.protobuf.utils import check_message_type

#===============================================================================
# Client class

class Client (API, SignalClient):
    '''
    Client for Demo service.
    '''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `messaging.grpc.Client` base to instantiate `self.stub`.
    from .demo_service_pb2_grpc import DemoStub as Stub

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
                 host           : str = "",        # gRPC server
                 identity       : str|None = None, # Greeter identity
                 wait_for_ready : bool = False,    # Keep trying to connect
                 use_asyncio    : bool = False,    # Use Python AsyncIO semantics
                 product_name   : str|None = None, # Short product name
                 project_name   : str|None = None, # Top-level code project
                 ):

        '''
        Initializer.  Parameters:

        @param host
            Server host and/or port number, in the form `address:port`.
            `address` may be a hostname or an IPv4 or IPv6 address string.  If
            either address or host is missing, the default value is obtain from
            the service settings file (grpc-endpoints-PRODUCT.json).

        @param identity
            Unique identity of this client, used to construct greetings.

        @param wait_for_ready
            If a connection attempt fails, keep retrying until successful.
            This value may be overriden per call.

        @param use_asyncio
            Use Python AsyncIO.  Effectively this performs calls within a
            `grpc.aio.Channel` instance, rather than the default `grpc.Channel`.
            Additionally, the `call()` method uses AsyncIO semantics to capture
            any exceptions.  If not specified, the default value is obtained
            from the corresponding `use_asyncio` class variable.

        @param product_name
            Name of the product, used to locate corresponding settings files
            (e.g. `grpc-endpoints-PRODUCT.yaml`).

        @param project_name
            Name of code project (e.g. parent code repository). Used to locate
            corresponding settings files (e.g., `grpc-endpoints-PROJECT.yaml`)
        '''



        API.__init__(self, 'Python gRPC client', identity)
        SignalClient.__init__(self,
                              host = host,
                              wait_for_ready = wait_for_ready,
                              use_asyncio = use_asyncio,
                              signal_store = demo_signals,
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
        self.stub.SayHello(greeting)

    def get_current_time(self) -> TimeData:
        '''
        Ask for the current time (local, utc, epoch) from the server.
        '''
        return self.stub.GetCurrentTime(empty)

    def start_ticking(self) -> None:
        '''
        Tell the server to start emitting `signal_time` once per second.
        '''
        return self.stub.StartTicking(empty)

    def stop_ticking(self) -> None:
        '''
        Tell the server to stop emitting `singal_time`.
        '''
        return self.stub.StopTicking(empty)

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


def main():
    print("Loaded DEMO main")



if __name__ == '__main__':
    client = Client()
    client.initialize()
