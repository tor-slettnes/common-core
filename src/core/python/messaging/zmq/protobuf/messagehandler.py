#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagehandler.py
## @brief Process received message publications with ProtoBuf payload
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from ..basic import Filter, Topic, Subscriber, MessageHandler as BaseHandler
from cc.protobuf.wellknown import Message, MessageType

class MessageHandler (BaseHandler):
    '''
    ZMQ subscriber with support for ProtoBuf messages
    '''

    ### Subclasses should override this with a ProtoBuf message type,
    ### or else provide the `message_type` argument to `__init__()`.
    message_type = None

    def __init__(self,
                 message_type : MessageType|None = None,
                 subscriber: Subscriber|None = None,
                 filter_or_topic: str|bytes|None = None):

        self.message_type = message_type or type(self).message_type
        assert isinstance(self.message_type, MessageType)

        if filter_or_topic is None:
            filter_or_topic = self.message_type.DESCRIPTOR.full_name

        BaseHandler.__init__(self,
                             id = self.message_type.DESCRIPTOR.full_name,
                             subscriber = subscriber,
                             filter_or_topic = filter_or_topic)

    def handle(self, data: bytes):
        '''
        Deserialize incoming binary payload as an instance of
        `self.message_type` and pass the result on to `self.handle_proto()`.
        '''

        self.handle_proto(self.message_type.FromString(data))


    ### Subclasses must override _one_ of the following two methods
    ### to process incoming ProtoBuf payloads

    def handle_proto(self, message: Message):
        '''
        Process incoming ProtoBuf message.

        Unless overridden in subclasses this further decodes the message to a
        Python dictionary and passes the result to
        `self.handle_proto_as_dict()`, which must in that case be overridden.

        In other words, subclasses should override either this method or
        `handle_proto_as_dict()` to process incoming messages.
        '''

        self.handle_proto_as_dict(
            protobuf.utils.decodeToDict(message, use_integers_for_enums = True))


    def handle_proto_as_dict(self, message : dict):
        '''
        Handle incoming ProtoBuf payload, decodeed as a Python dictionary
        '''

        raise NotImplementedError
