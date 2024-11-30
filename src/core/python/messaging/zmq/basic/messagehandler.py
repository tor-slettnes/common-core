#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagehandler.py
## @brief Process received message publications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .filter import Filter, Topic

### Standard Python modules
from typing import Union

class MessageHandler (object):
    '''
    Handle message publications for a specific filter/payload type.
    '''

    def __init__(self,
                 id : str,
                 filter_or_topic : Union[Filter, Topic] = Filter(b'')):

        self.id = id

        if filter_or_topic is None:
            self.message_filter = Filter(b'')
        elif isinstance(filter_or_topic, Filter):
            self.message_filter = filter_or_topic
        elif isinstance(filter_or_topic, (bytes, bytearray)):
            self.message_filter = Filter(filter_or_topic)
        elif isinstance(filter_or_topc, str):
            self.message_filter = Filter.create_from_topic(filter_or_topic)
        else:
            raise TypeError("`filter_or_topic' must be a bytes, bytearray, or string")

    def initialize(self):
        pass

    def deinitialize(self):
        pass

    def handle (self, data:bytes):
        '''
        Handle payload of received messages
        '''
        raise NotImplementedError
