#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file publisher.py
## @brief ZeroMQ publisher base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .host   import Host
from .filter import Filter, Topic

### Third-party modules
import zmq

class Publisher (Host):
    endpoint_type = 'publisher'

    def __init__(self,
                 bind_address: str,
                 channel_name: str = None):

        Host.__init__(self, bind_address, channel_name, zmq.PUB)

    def publish(self,
                message_filter : Filter,
                data : bytes):

        if len(message_filter):
            self.send_bytes(message_filter, zmq.DONTWAIT | zmq.SNDMORE)
        self.send_bytes(data, zmq.DONTWAIT)
