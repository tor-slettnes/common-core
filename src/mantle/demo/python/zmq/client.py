#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes
#===============================================================================

### Modules relative to current dir
from .common import DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE
from ..base  import API, demo_signals
from ..protobuf import Greeting, TimeData

### Modules relative to install dir
from cc.messaging.zmq.basic.subscriber import Subscriber
from cc.messaging.zmq.protobuf.client import Client as ProtoBufClient
from cc.messaging.zmq.protobuf.signalhandler import SignalHandler
from cc.protobuf.utils import check_message_type

### Standard Python modules
import time, sys, os.path


#===============================================================================
# Client class

class Client (API, ProtoBufClient):
    '''
    Client for Demo service.
    '''

    def __init__(self,
                 service_host: str = "",
                 publisher_host: str = "",
                 identity: str = None):

        API.__init__(self, 'Python ZMQ Client', identity);
        ProtoBufClient.__init__(
            self,
            host_address = service_host,
            channel_name = DEMO_SERVICE_CHANNEL,
            interface_name = DEMO_RPC_INTERFACE)

        self.subscriber = Subscriber(
            host_address = publisher_host,
            channel_name = DEMO_PUBLISHER_CHANNEL,
            product_name = self.product_name)

        self.signalhandler = SignalHandler(
            signal_store = demo_signals,
            subscriber = self.subscriber
        )


    def initialize(self):
        '''
        Initialize client/server connection
        '''
        self.signalhandler.initialize()
        self.subscriber.initialize()
        super().initialize()

    def deinitialize(self):
        '''
        Deinitialize client/server connection
        '''
        super().definitialize()
        self.subscriber.deinitialize()
        self.signalhandler.deinitialize()

    def say_hello(self, greeting: Greeting):
        '''
        Send a greeting to the server.

        @param greeting:
            A greeting object.

        See also `hello()` if you want to pass in just the text of the greeting
        rather than constructing your own `Greeting` object.
        '''

        check_message_type(greeting, Greeting)
        self.call('say_hello', greeting)

    def get_current_time(self) -> TimeData:
        '''
        Ask for the current time (local, utc, epoch) from the server.
        '''
        return self.call('get_current_time', None, TimeData)

    def start_ticking(self) -> None:
        '''
        Tell the server to start emitting `signal_time` once per second.
        '''

        return self.call('start_ticking')

    def stop_ticking(self) -> None:
        '''
        Tell the server to stop emitting `singal_time`.
        '''
        return self.call('stop_ticking')


if __name__ == '__main__':
    client = Client()
    client.initialize()
