#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalhandler.py
## @brief Subscribe to Signal messages and re-emit locally as signals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from cc.protobuf.signal import SignalStore
from cc.protobuf.wellknown import Message

from ..basic import Subscriber
from .messagehandler import MessageHandler


class SignalHandler (MessageHandler):
    '''
    Subscribe to `Signal` messages from ZMQ publisher and re-emit locally as
    signals.
    '''

    def __init__(self,
                 signal_store: SignalStore,
                 subscriber: Subscriber|None = None):

        MessageHandler.__init__(self,
                                subscriber = subscriber,
                                message_type = signal_store.signal_type)
        self.signal_store = signal_store

    def handle_proto(self, message: Message):
        self.signal_store.emit(message)

