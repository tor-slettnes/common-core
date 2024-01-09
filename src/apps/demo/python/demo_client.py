#!/usr/bin/python3 -i
#===============================================================================
## @file demo_client.py
## @brief Python client for `Demo` service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

import time
import grpc_signalclient

from scalar_types  import enums
from typing        import Callable

class CC (grpc_signalclient.CC):
    import demo_types_pb2 as Demo

#===============================================================================
# Enumerated values

Weekday = enums(CC.Demo.Weekday)

#===============================================================================
# SignalClient class

class DemoClient (grpc_signalclient.SignalClient):
    '''Client for Demo service.'''

    from demo_service_pb2_grpc import DemoStub as stub
    servicename="Demo"

    def __init__(self,
                 host = "",
                 identity = "Interactive",
                 servicename = servicename,
                 wait_for_ready = False):

        super().__init__(host = host,
                         servicename = servicename,
                         SignalType = CC.Demo.Signal,
                         watch_all = True)

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

    def start_notify_greetings(self,
                               callback: Callable[[CC.Demo.Greeting], None],
                               prepend: tuple = (),
                               append: tuple = (),
                               **kwargs):
        '''
        Register a callback to be invoked whenever a greeting is received

        @param[in] callback
            Callback method, which will receive the greeting as argument

        @param[in] prepend
            Additional callback arguments to prepend before greeting

        @param[in] append
            Additional callback arguments to append after greeting

        @param[in] kwargs
            Additional keyword arguments to pass to callback function

        '''

        self.connect_signal('greeting', callback, prepend, append, kwargs)
        self.start_watching()


    def stop_notify_greetings(self):
        '''
        Unregister any existing greeting callback
        '''
        self.disconnect_signal('greeting')


    def start_notify_time(self,
                          callback: Callable[[CC.Demo.TimeData], None],
                          prepend: tuple = (),
                          append: tuple = (),
                          **kwargs):
        '''
        @brief Register a callback to be invoked whenver the time updated (1x/second)

        @param[in] callback
            Callback method, which will receive the time data as argument.

        @param[in] prepend
            Additional callback arguments to prepend before the time data

        @param[in] append
            Additional callback arguments to append after the time data

        @param[in] kwargs
            Additional keyword arguments to pass to callback function
        '''

        self.connect_signal('time', callback, prepend, append, kwargs)
        self.start_watching()


    def stop_notify_time (self):
        '''
        @brief Unregister any existing time callbacks
        '''

        self.disconnect_signal('time')
