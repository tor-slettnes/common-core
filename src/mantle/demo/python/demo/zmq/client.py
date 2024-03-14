#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo_zmq_client.py
## @brief Python client for `Demo` ZMQ RPC service
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install dir
from .common import DEMO_PUBLISHER_CHANNEL, DEMO_SERVICE_CHANNEL, DEMO_RPC_INTERFACE
from ..core  import API, CC, ProtoBuf, SignalSlot, demo_signals
from cc.messaging.zmq.basic import Subscriber
from cc.messaging.zmq.protobuf import Client, SignalHandler

### Standard Python modules
from typing import Callable
import time, sys, os.path


#===============================================================================
# DemoClient class

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

        self.subscriber = Subscriber(
            host_address = publisher_host,
            channel_name = DEMO_PUBLISHER_CHANNEL)

        self.signalhandler = SignalHandler(demo_signals)


    def connect(self):
        self.subscriber.connect()
        self.subscriber.add(self.signalhandler)
        Client.connect(self)

    def disconnect(self):
        Client.disconnect(self)
        self.subscriber.remove(self.signalhandler)
        self.subscriber.disconnect()

    def say_hello(self, greeting: CC.Demo.Greeting):
        self.check_type(greeting, CC.Demo.Greeting)
        self.call('say_hello', greeting)

    def get_current_time(self) -> CC.Demo.TimeData:
        return self.call('get_current_time', None, CC.Demo.TimeData)

    def start_ticking(self) -> None:
        return self.call('start_ticking')

    def stop_ticking(self) -> None:
        return self.call('stop_ticking')
