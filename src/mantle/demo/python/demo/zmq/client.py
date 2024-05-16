#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_zmq_client.py
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
import cc.protobuf.demo
import cc.protobuf.utils

### Standard Python modules
import time, sys, os.path


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

    def say_hello(self, greeting: cc.protobuf.demo.Greeting):
        cc.protobuf.utils.check_type(greeting, cc.protobuf.demo.Greeting)
        self.call('say_hello', greeting)

    def get_current_time(self) -> cc.protobuf.demo.TimeData:
        return self.call('get_current_time', None, cc.protobuf.demo.TimeData)

    def start_ticking(self) -> None:
        return self.call('start_ticking')

    def stop_ticking(self) -> None:
        return self.call('stop_ticking')
