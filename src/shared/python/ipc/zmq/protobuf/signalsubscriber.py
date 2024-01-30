#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file signalsubscriber.py
## @brief ZeroMQ subscriber with local signal emission
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from .subscriber              import Subscriber
from ...common.signalreceiver import SignalReceiver
from ...google.protobuf       import CC, ProtoBuf

class SignalSubscriber (Subscriber, SignalReceiver):
    '''ZMQ ProtoBuf subscriber local signal emission'''

    signal_type = None

    def __init__(self,
                 host_address : str,
                 use_cache    : bool = False,
                 channel_name : str = None,
                 signal_type  : ProtoBuf.MessageType = None):

        Subscriber.__init__(self, host_address, channel_name)
        SignalReceiver.__init__(self,
                                use_cache   = use_cache,
                                signal_type = signal_type)


    def start_watching (self):
        self.subscribe_proto(self.signal_type, self.process_signal_message)

    def stop_watching (self):
        self.unsubscribe()
