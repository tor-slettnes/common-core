#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalhandler.py
## @brief Subscribe to Signal messages and re-emit locally as signals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to current folder
from .messagehandler import MessageHandler

### Modules relative to install folder
import cc.protobuf.signal
import cc.protobuf.wellknown

class SignalHandler (MessageHandler):
    '''Subscribe to `Signal` messages from ZMQ publisher and re-emit locally as signals.'''

    def __init__(self,
                 signal_store: cc.protobuf.signal.SignalStore):

        MessageHandler.__init__(self, signal_store.signal_type)
        self.signal_store = signal_store

    def handle_proto(self, message: cc.protobuf.wellknown.Message):
        self.signal_store.emit(message)

