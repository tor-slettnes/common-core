#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file publisher.py
## @brief ZeroMQ publisher - ProtoBuf flavor
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..basic.publisher  import Publisher as BasePublisher
from ...google.protobuf import ProtoBuf

class Publisher (BasePublisher):
    '''ZMQ publisher with support for ProtoBuf messages'''

    def publish_proto(self,
                      message : ProtoBuf.Message):
        '''Publish a ProtoBuf message over ZMQ'''

        self.publish_raw(message.SerializeToString())


    def publish_proto_from_dict(self,
                                prototype: ProtoBuf.MessageType,
                                data     : dict,
                                ignore_unknown_fields: bool = False):
        '''
        Encode a ProtoBuf message from a corresponding parseable dictionary,
        and publish over ZMQ
        '''

        self.publish_proto(
            ProtoBuf.encodeFromDict(data, prototype, ignore_unknown_fields))
