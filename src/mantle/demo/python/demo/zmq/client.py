#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to current dir
from .common import DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE
from ..base  import API, demo_signals, PROJECT_NAME

### Modules relative to install dir
from cc.messaging.zmq.basic.subscriber import Subscriber
from cc.messaging.zmq.protobuf.client import Client as BaseClient
from cc.messaging.zmq.protobuf.signalhandler import SignalHandler
from cc.protobuf.demo import Greeting, TimeData
from cc.protobuf.utils import check_message_type

### Standard Python modules
import time, sys, os.path


#===============================================================================
# Client class

class Client (API, BaseClient):
    '''
    Client for Demo service.
    '''

    def __init__(self,
                 service_host: str = "",
                 publisher_host: str = "",
                 identity: str = None):

        API.__init__(self, 'Python ZMQ Client', identity);
        BaseClient.__init__(self,
                            host_address = service_host,
                            channel_name = DEMO_SERVICE_CHANNEL,
                            interface_name = DEMO_RPC_INTERFACE,
                            project_name = PROJECT_NAME)

        self.subscriber = Subscriber(
            host_address = publisher_host,
            channel_name = DEMO_PUBLISHER_CHANNEL,
            project_name = PROJECT_NAME)

        self.signalhandler = SignalHandler(demo_signals)


    def connect(self):
        '''
        Establish connection to the server, and start listening for signals.
        '''
        self.subscriber.connect()
        self.subscriber.add(self.signalhandler)
        BaseClient.connect(self)

    def disconnect(self):
        '''
        Disconnect from the server. Unregister signal handler.
        '''

        BaseClient.disconnect(self)
        self.subscriber.remove(self.signalhandler)
        self.subscriber.disconnect()

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
