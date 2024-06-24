#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagewriter.py
## @brief ZeroMQ message writer - ProtoBuf flavor
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..basic import Publisher, Filter, MessageWriter as BaseWriter
from ....protobuf.wellknown import Message, MessageType
from ....protobuf.utils import dictToMessage


class MessageWriter (BaseWriter):
    '''ZMQ publisher with support for ProtoBuf messages'''

    ### Subclasses should override this with a ProtoBuf message type,
    ### or else provide the `message_type` argument to `__init__()`.
    message_type = None

    def __init__ (self,
                  publisher: Publisher,
                  message_type: MessageType|None = None):

        self.message_type = message_type or type(self).message_type
        assert isinstance(self.message_type, MessageType)

        topic = self.message_type.DESCRIPTOR.full_name
        BaseWriter.__init__(self, publisher, Filter.create_from_topic(topic))

    def write_proto(self,
                    message : Message):
        '''Publish a ProtoBuf message over ZMQ'''

        self.write_bytes(message.SerializeToString())

    def write_dict_as_proto(self,
                            prototype: MessageType,
                            data : dict,
                            ignore_unknown_fields: bool = False):
        '''
        Encode a ProtoBuf message from a corresponding parseable dictionary,
        and publish over ZMQ
        '''

        self.write_proto(
            dictToMessage(data, prototype, ignore_unknown_fields))
