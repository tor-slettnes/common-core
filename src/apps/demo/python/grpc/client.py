#!/usr/bin/python3 -i
#===============================================================================
## @file demo_grpc_client.py
## @brief Python client for `Demo` gRPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..common    import CC, ProtoBuf, Weekdays
from typing      import Callable
from ipc.google.grpc.signalclient import SignalClient, \
    SimpleSignalHandler, MappingSignalHandler

import time, sys, os.path


#===============================================================================
# SignalClient class

class DemoClient (SignalClient):
    '''Client for Demo service.'''

    from demo_service_pb2_grpc import DemoStub as stub
    service_name = "CC.Demo.Demo"
    signal_type  = CC.Demo.Signal

    def __init__(self,
                 host           : str = "",
                 identity       : str = os.path.basename(sys.argv[0]),
                 wait_for_ready : bool = False):

        SignalClient.__init__(self, host = host, watch_all = True)

        self.identity = identity
        self.birth    = time.time()


    def say_hello(self, text: str, **kwargs):
        '''
        @brief  Issue a greeting to anyone who may be listening
        @param[in] text
            What we're telling our listeners
        '''

        request = CC.Demo.Greeting(text=text,
                                   identity=self.identity,
                                   implementation=type(self).__name__,
                                   birth=CC.encodeTimestamp(self.birth),
                                   data=CC.encodeValueList(kwargs))
        self._wrap(self.stub.say_hello, request)

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


    def start_notify_greetings(self, callback: MappingSignalHandler):
        '''
        Register a callback to be invoked whenever a greeting is received

        @param[in] callback
            Callback method, which will receive three arguments:
             * The mapping change: MAP_ADDITION, MAP_UPDATE, or MAP_REMOVAL
             * The mapping key: the identity of the greeter
             * The new/updated greeting, if any
        '''

        self.connect_mapping_signal('greeting', callback)
        self.start_watching()


    def stop_notify_greetings(self):
        '''
        Unregister any existing greeting callback
        '''
        self.disconnect_signal('greeting')


    def start_notify_time(self, callback: SimpleSignalHandler):
        '''
        @brief Register a callback to be invoked whenver the time updated (1x/second)

        @param[in] callback
            Callback method, which will receive the time data as argument.
        '''

        self.connect_signal('time', callback)
        self.start_watching()


    def stop_notify_time (self):
        '''
        @brief Unregister any existing time callbacks
        '''

        self.disconnect_signal('time')
