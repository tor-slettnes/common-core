#!/bin/echo Do not invoke directly.
#===============================================================================
## @file client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to current dir
from .common import DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE
from ..core  import API, signal_store

### Modules relative to install dir
from cc.messaging.zmq.basic.subscriber import Subscriber
from cc.messaging.zmq.protobuf.client import Client as BaseClient
from cc.messaging.zmq.protobuf.signalhandler import SignalHandler
from cc.protobuf.import_proto import import_proto
from cc.protobuf.utils import check_message_type

### Standard Python modules
import time, sys, os.path

## Import symbols generted from `demo.proto`. These will appear in the namespace
## `cc.demo`.
import_proto('demo', globals())

#===============================================================================
# DemoClient class

class DemoClient (API, BaseClient):
    '''Client for Demo service.'''

    def __init__(self,
                 service_host: str = "",
                 publisher_host: str = "",
                 identity: str = None):

        API.__init__(self, 'Python ZMQ Client', identity);
        BaseClient.__init__(self,
                            host_address = service_host,
                            channel_name = DEMO_SERVICE_CHANNEL,
                            interface_name = DEMO_RPC_INTERFACE)

        self.subscriber = Subscriber(
            host_address = publisher_host,
            channel_name = DEMO_PUBLISHER_CHANNEL)

        self.signalhandler = SignalHandler(signal_store)


    def connect(self):
        self.subscriber.connect()
        self.subscriber.add(self.signalhandler)
        BaseClient.connect(self)

    def disconnect(self):
        BaseClient.disconnect(self)
        self.subscriber.remove(self.signalhandler)
        self.subscriber.disconnect()

    def say_hello(self, greeting: cc.demo.Greeting):
        check_message_type(greeting, cc.demo.Greeting)
        self.call('say_hello', greeting)

    def get_current_time(self) -> cc.demo.TimeData:
        return self.call('get_current_time', None, cc.demo.TimeData)

    def start_ticking(self) -> None:
        return self.call('start_ticking')

    def stop_ticking(self) -> None:
        return self.call('stop_ticking')
