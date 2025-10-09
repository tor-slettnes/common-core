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
from .messagebase import MessageBase


class MessageHandler (MessageBase):
    '''
    Handle message publications for a specific filter/payload type.
    '''

    def __init__(self,
                 id             : str|None = None,
                 subscriber     : Optional["Subscriber"] = None,
                 message_filter : bytes|str|None = None):

        self.id = id
        self.subscriber_ref = weakref.ref(subscriber) if subscriber else None
        self.message_filter = self.encode_topic(message_filter)

    def initialize(self):
        if self.subscriber_ref:
            if subscriber := self.subscriber_ref():
                subscriber.add(self)

    def deinitialize(self):
        if self.subscriber_ref:
            if subscriber := self.subscriber_ref():
                subscriber.remove(self)

    # Subclasses should implement one of the following two methods to process
    # incoming message publications.

    def handle_parts(self,
                     parts: list[bytes]):
        '''
        Handle incoming ZMQ message as a list of parts.
        '''

        payload = self.combine_parts(
            parts,
            remove_header = self.message_filter is not None)

        self.handle(payload)


    def handle (self, data:bytes):
        '''
        Handle incoming ZMQ message as a flat bytes instance combined from
        the message parts.

        The first part is included only if `message_filter` is None, in which
        case this handler will be invoked for every received ZMQ message.

        The first part is skipped if `message_filter` is a valid `bytes`
        instance, since this handler would then only be invoked if these are
        identical.
        '''
        raise NotImplementedError


    def combine_parts(self,
                      parts: list[bytes],
                      remove_header: bool = True) -> bytes:
        return b''.join(parts[remove_header:])
