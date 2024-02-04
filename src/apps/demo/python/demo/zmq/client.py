#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_zmq_client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .common import DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE
from ..core  import API, CC, ProtoBuf, demo_signals
from ipc.zmq.protobuf.client import Client
from ipc.zmq.protobuf.signalsubscriber import SignalSubscriber

### Standard Python modules
from typing import Callable
import time, sys, os.path


#===============================================================================
# SignalClient class

class DemoClient (API, Client):
    '''Client for Demo service.'''

    def __init__(self,
                 service_host: str = "",
                 publisher_host: str = "",
                 identity: str = None):

        API.__init__(self, 'Python ZMQ Client', identity);
        Client.__init__(self,
                        host_address = service_host,
                        channel_name = DEMO_SERVICE_CHANNEL,
                        interface_name = DEMO_RPC_INTERFACE)

        self.subscriber = SignalSubscriber(
            host_address = publisher_host,
            signal_store = demo_signals,
            channel_name = DEMO_PUBLISHER_CHANNEL)

    def connect(self):
        self.subscriber.connect()
        self.subscriber.start_watching()
        Client.connect(self)

    def disconnect(self):
        Client.disconnect(self)
        self.subscriber.stop_watching()
        self.subscriber.disconnect()

    def say_hello(self, greeting: CC.Demo.Greeting):
        '''
        @brief  Issue a greeting to anyone who may be listening
        @param[in] greeting
            A greeting for our listeners.
        '''
        self.call('say_hello', greeting)

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

