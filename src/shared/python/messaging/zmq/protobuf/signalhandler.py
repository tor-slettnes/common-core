#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalhandler.py
## @brief Subscribe to Signal messages and re-emit locally as signals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to install folder
from .messagehandler import MessageHandler
from ...common import SignalStore
from ...protobuf import CC, ProtoBuf

class SignalHandler (MessageHandler):
    '''Subscribe to `Signal` messages from ZMQ publisher and re-emit locally as signals.'''

    def __init__(self,
                 signal_store: SignalStore):

        MessageHandler.__init__(self, signal_store.signal_type)
        self.signal_store = signal_store

    def handle_proto(self, message: ProtoBuf.Message):
        self.signal_store.emit(message)

