#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file publisher.py
## @brief ZeroMQ publisher base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .endpoint import Endpoint
from .messagebase import MessageBase

### Third-party modules
import zmq

class Publisher (Endpoint, MessageBase):
    endpoint_type = 'publisher'

    def __init__(self,
                 bind_address: str,
                 channel_name: str|None = None,
                 project_name: str|None = None,
                 product_name: str|None = None,
                 role: Endpoint.Role = Endpoint.Role.HOST,
                 ):

        Endpoint.__init__(
            self,
            address = bind_address,
            channel_name = channel_name,
            project_name = project_name,
            product_name = product_name,
            socket_type = zmq.PUB,
            role = role)

    def publish(self,
                topic: str|bytes|bytearray|None,
                data : bytes):
        '''
        Publish a binary payload, with an optional header/topic.
        '''

        if topic is not None:
            header = self.encode_topic(topic)
            self.send_bytes(header, zmq.DONTWAIT | zmq.SNDMORE)

        self.send_bytes(data, zmq.DONTWAIT)
