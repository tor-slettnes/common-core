#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file publisher.py
## @brief ZeroMQ publisher base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .host   import Host
from .filter import Filter, Topic

### Third-party modules
import zmq

class Publisher (Host):
    endpoint_type = 'publisher'

    def __init__(self,
                 bind_address: str,
                 channel_name: str|None = None,
                 project_name: str|None = None,
                 ):

        Host.__init__(self,
                      address = bind_address,
                      channel_name = channel_name,
                      project_name = project_name,
                      socket_type = zmq.PUB)

    def publish(self,
                message_filter : Filter,
                data : bytes):

        if len(message_filter):
            self.send_bytes(message_filter, zmq.DONTWAIT | zmq.SNDMORE)
        self.send_bytes(data, zmq.DONTWAIT)
