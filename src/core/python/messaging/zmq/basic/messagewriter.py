#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagewriter.py
## @brief Publish messages with optional topic filter
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .filter import Filter, Topic
from .publisher import Publisher

### Standard Python modules
from typing import Union

class MessageWriter (object):
    def __init__(self,
                 publisher : Publisher,
                 filter_or_topic : Union[Filter, Topic] = Filter(b'')):

        self.publisher = publisher

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

    def write_bytes(self, data : bytes):
        '''Publish binary data with prepended message filter'''
        self.publisher.publish(self.message_filter, data)
