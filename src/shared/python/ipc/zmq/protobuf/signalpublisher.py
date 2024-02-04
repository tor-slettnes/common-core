#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalpublisher.py
## @brief ZeroMQ publisher that captures and forwards local signals
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .publisher import Publisher
from ...base.signalstore import SignalStore

class SignalPublisher (Publisher):
    '''ZMQ ProtoBuf subscriber local signal emission'''

    def __init__(self,
                 bind_address : str,
                 signal_store : SignalStore,
                 channel_name : str = None):

        Publisher.__init__(self, bind_address, channel_name)
        self.signal_store = signal_store
        self.started = False

    def start (self):
        if not self.started:
            for signal_name in self.signal_store.signal_names():
                self.signal_store.connect_signal(signal_name, self.publish_proto)
            self.started = True

    def stop (self):
        if self.started:
            for signal_name in self.signal_store.signal_names():
                self.signal_store.disconnect_signal(signal_name, self.publish_proto)
            self.started = False
