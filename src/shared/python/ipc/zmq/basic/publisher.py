#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file publisher.py
## @brief ZeroMQ publisher base
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .host   import Host
from .filter import Filter

import zmq

TopicType = str

class Publisher (Host):
    endpoint_type = 'publisher'

    def __init__(self,
                 bind_address: str,
                 channel_name: str = None):

        Host.__init__(self, bind_address, channel_name, zmq.PUB)

    def publish_raw(self, data : bytes):
        self.send_bytes(data, zmq.DONTWAIT)

    def publish_with_topic(self,
                           topic: TopicType,
                           data : bytes):
        f = Filter.create_from_topic(topic)
        self.publish_with_filter(f, data)

    def publish_with_filter(self,
                            filter: bytes,
                            data  : bytes):
        if len(filter):
            self.send_bytes(filter, zmq.DONTWAIT | zmq.SNDMORE)
        self.send_bytes(data, zmq.DONTWAIT)
