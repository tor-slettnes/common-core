#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagehandler.py
## @brief Process received message publications with ProtoBuf payload
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..basic import Filter, Topic, MessageHandler as BaseHandler
from cc.protobuf.wellknown import Message, MessageType

class MessageHandler (BaseHandler):
    '''ZMQ subscriber with support for ProtoBuf messages'''

    ### Subclasses should override this with a ProtoBuf message type,
    ### or else provide the `message_type` argument to `__init__()`.
    message_type = None

    def __init__(self,
                 message_type : MessageType|None = None):

        self.message_type = message_type or type(self).message_type
        assert isinstance(self.message_type, MessageType)

        topic = self.message_type.DESCRIPTOR.full_name
        BaseHandler.__init__(self, topic, Filter.create_from_topic(topic))

    def handle(self, data: bytes):
        '''Deserialize incoming binary payload as an instance of
         `self.message_type` and pass it on to `self.handle_proto()`.

        '''
        self.handle_proto(self.message_type.FromString(data))


    ### Subclasses must override _one_ of the following two methods
    ### to process incoming ProtoBuf payloads

    def handle_proto(self, message: Message):
        '''Process incoming ProtoBuf message.

        Unless overridden in subclasses this further decodes the message to a
        Python dictionary and passes the result to
        `self.handle_proto_as_dict()`, which must in that case be overridden.
        '''

        self.handle_proto_as_dict(
            protobuf.utils.decodeToDict(message, use_integers_for_enums = True))


    def handle_proto_as_dict(self, message : dict):
        '''Handle incoming ProtoBuf payload, decodeed as a Python dictionary'''

        raise NotImplementedError
