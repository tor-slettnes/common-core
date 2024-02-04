#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalsubscriber.py
## @brief ZeroMQ subscriber with local signal emission
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .subscriber import Subscriber
from ...base.signalstore import SignalStore
from ...google.protobuf import CC, ProtoBuf

class SignalSubscriber (Subscriber):
    '''ZMQ ProtoBuf subscriber local signal emission'''

    def __init__(self,
                 host_address : str,
                 signal_store : SignalStore,
                 channel_name : str = None):

        Subscriber.__init__(self, host_address, channel_name)
        self.signal_store = signal_store
        self.watching = False


    def start_watching (self):
        if not self.watching:
            self.subscribe_proto(self.signal_store.signal_type, self.signal_store.emit)
            self.watching = True

    def stop_watching (self):
        if self.watching:
            self.unsubscribe()
            self.watching = False
