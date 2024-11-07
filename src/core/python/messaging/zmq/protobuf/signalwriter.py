#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalwriter.py
## @brief Forward local signals to ZeroMQ publisher
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .messagewriter import MessageWriter
from ..basic import Publisher
from cc.protobuf.signal import SignalStore

class SignalWriter (MessageWriter):
    '''Capture local signals and forward to remote peers as ZMQ publications'''

    def __init__(self,
                 publisher : Publisher,
                 signal_store : SignalStore):

        MessageWriter.__init__(self, publisher, signal_store.signal_type)
        self.signal_store = signal_store
        self.started = False

    def start (self):
        if not self.started:
            self.signal_store.connect_all(self.write_proto)
            self.started = True

    def stop (self):
        if self.started:
            self.signal_store.disconnect_all(self.write_proto)
            self.started = False
