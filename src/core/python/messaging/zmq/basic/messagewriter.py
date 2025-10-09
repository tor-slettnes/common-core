#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagewriter.py
## @brief Publish messages with optional topic filter
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .publisher   import Publisher
from .messagebase import MessageBase

### Standard Python modules
from typing import Union

class MessageWriter (MessageBase):
    def __init__(self,
                 publisher: Publisher,
                 topic    : bytes|str|None = None):

        self.publisher = publisher
        self.topic     = self.encode_topic(topic)

    def initialize(self):
        pass

    def deinitialize(self):
        pass

    def write_bytes(self, data : bytes):
        '''
        Publish binary data with prepended message filter
        '''
        self.publisher.publish(self.topic, data)
