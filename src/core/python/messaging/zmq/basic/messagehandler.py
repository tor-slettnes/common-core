#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagehandler.py
## @brief Process received message publications
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
from typing import Union, Optional
import weakref

### Modules within package
from .filter import Filter, Topic


class MessageHandler (object):
    '''
    Handle message publications for a specific filter/payload type.
    '''

    def __init__(self,
                 id : str|None = None,
                 subscriber: Optional["Subscriber"] = None,
                 filter_or_topic : Union[Filter,Topic,None] = None):

        self.id = id
        self.subscriber_ref = weakref.ref(subscriber) if subscriber else None

        if filter_or_topic is None:
            self.message_filter = Filter(b'')
        elif isinstance(filter_or_topic, Filter):
            self.message_filter = filter_or_topic
        elif isinstance(filter_or_topic, (bytes, bytearray)):
            self.message_filter = Filter(filter_or_topic)
        elif isinstance(filter_or_topic, str):
            self.message_filter = Filter.create_from_topic(filter_or_topic)
        else:
            raise TypeError("`filter_or_topic' must be a bytes, bytearray, or string")

        self.initialize()

    def __del__(self):
        self.deinitialize()

    def initialize(self):
        if self.subscriber_ref:
            if subscriber := self.subscriber_ref():
                subscriber.add(self)

    def deinitialize(self):
        if self.subscriber_ref:
            if subscriber := self.subscriber_ref():
                subscriber.remove(self)


    def handle (self, data:bytes):
        '''
        Handle payload of received messages
        '''
        raise NotImplementedError
