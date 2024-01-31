#!/usr/bin/python3 -i
#===============================================================================
## @file demo_zmq_client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..common import CC, ProtoBuf, Weekdays, \
    DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE

from ipc.zmq.protobuf.client import Client
from ipc.zmq.protobuf.signalsubscriber import SignalSubscriber
from typing import Callable

import time, sys, os.path

#===============================================================================
# SignalClient class

class DemoClient (Client):
    '''Client for Demo service.'''

    def __init__(self,
                 service_host: str = "",
                 publisher_host: str = "",
                 identity = os.path.basename(sys.argv[0])):

        Client.__init__(self,
                        host_address = service_host,
                        channel_name = DEMO_SERVICE_CHANNEL,
                        interface_name = DEMO_RPC_INTERFACE)

        self.subscriber = SignalSubscriber(
            host_address = publisher_host,
            channel_name = DEMO_PUBLISHER_CHANNEL,
            signal_type  = CC.Demo.Signal)

        self.identity = identity
        self.birth    = time.time()

    def connect(self):
        self.subscriber.connect()
        Client.connect(self)

    def disconnect(self):
        Client.disconnect(self)
        self.subscriber.disconnect()

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
        self.call('say_hello', request)



    def get_current_time(self) -> CC.Demo.TimeData:
        '''
        Get current time data.
        @return
            Current time data provided by the specific implementation.
        '''

        return self.call('get_current_time', None, CC.Demo.TimeData)

    def start_ticking(self) -> None:
        '''
        Tell the server to start issuing periodic time updates
        '''
        return self.call('start_ticking')


    def stop_ticking(self) -> None:
        '''
        Tell the server to stop issuing periodic time updates
        '''
        return self.call('stop_ticking')


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

        self.subscriber.connect_mapping_signal('greeting', callback)
        self.subscriber.start_watching()


    def stop_notify_greetings(self):
        '''
        Unregister any existing greeting callback
        '''

        self.subscriber.disconnect_signal('greeting')


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

        self.subscriber.connect_signal('time', callback)
        self.subscriber.start_watching()



    def stop_notify_time (self):
        '''
        @brief Unregister any existing time callbacks
        '''

        self.subscriber.disconnect_signal('time')
