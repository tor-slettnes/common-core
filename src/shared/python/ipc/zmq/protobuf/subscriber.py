#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file subscriber.py
## @brief ZeroMQ subscriber - ProtoBuf flavor
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..basic.subscriber import Subscriber as BaseSubscriber
from ...google.protobuf import ProtoBuf
from typing             import Callable

class Subscriber (BaseSubscriber):
    '''ZMQ subscriber with support for ProtoBuf messages'''

    def subscribe_proto(self,
                        prototype : ProtoBuf.MessageType,
                        callback  : Callable[[ProtoBuf.Message], None]):
        '''Subscribe to ZMQ messages with a ProtoBuf payload'''

        self.subscribe(
            lambda _data: callback(prototype.FromString(_data)))


    def subscribe_proto_as_dict(self,
                                prototype : ProtoBuf.MessageType,
                                callback  : Callable[[dict], None],
                                use_integers_for_enums : bool = True):
        '''Subscribe to ZMQ messages with a ProtoBuf payload,
        decoded as Python dictionaries.
        '''

        self.subscribe_proto(
            prototype,
            lambda _msg: callback(
                ProtoBuf.decodeToDict(_msg, use_integers_for_enums)))
