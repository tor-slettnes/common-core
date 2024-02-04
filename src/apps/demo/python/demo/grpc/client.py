#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_grpc_client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from ..core import API, CC, ProtoBuf, demo_signals
from ipc.grpc.signalclient import SignalClient


#===============================================================================
# SignalClient class

class DemoClient (API, SignalClient):
    '''Client for Demo service.'''

    ## `Stub` is the generated gRPC client Stub, and is used by the
    ## `ipc.grpc.Client` base to instantiate `self.stub`.
    from demo_service_pb2_grpc import DemoStub as Stub

    ## `service_name` is optional. If not provided here it is determined
    ## from the above stub.  It is used to look up service specific settings,
    ## such as target host/port.
    service_name = None

    def __init__(self,
                 host           : str = "",
                 identity       : str = None,
                 wait_for_ready : bool = False):

        API.__init__(self, 'Python gRPC client', identity)
        SignalClient.__init__(self,
                              host = host,
                              signal_store = demo_signals,
                              watch_all = True,
                              wait_for_ready = wait_for_ready)


    def say_hello(self, greeting: CC.Demo.Greeting):
        '''
        @brief  Issue a greeting to anyone who may be listening
        @param[in] greeting
            A greeting for our listeners.
        '''
        self._wrap(self.stub.say_hello, greeting)

    def get_current_time(self) -> CC.Demo.TimeData:
        '''
        Get current time data.
        @return
            Current time data provided by the specific implementation.
        '''
        return self._wrap(self.stub.get_current_time)

    def start_ticking(self) -> None:
        '''
        Tell the server to start issuing periodic time updates
        '''

        self._wrap(self.stub.start_ticking)

    def stop_ticking(self) -> None:
        '''
        Tell the server to stop issuing periodic time updates
        '''

        self._wrap(self.stub.stop_ticking)
