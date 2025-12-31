#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file messagehandler.py
## @brief Process received message publications with ProtoBuf payload
## @author Tor Slettnes
#===============================================================================

### Standard Python modules
from abc import abstractmethod

### Modules within package
from ..basic import Subscriber, MessageHandler as BaseHandler
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
                 message_filter: str|bytes|None = None):

        self.message_type = message_type or type(self).message_type
        assert isinstance(self.message_type, MessageType)

        if message_filter is None:
            message_filter = self.message_type.DESCRIPTOR.full_name

        BaseHandler.__init__(self,
                             id = self.message_type.DESCRIPTOR.full_name,
                             subscriber = subscriber,
                             message_filter = message_filter)

    def handle(self, data: bytes):
        '''
        Deserialize incoming binary payload as an instance of
        `self.message_type` and pass the result on to `self.handle_proto()`.
        '''

        self.handle_proto(self.message_type.FromString(data))


    @abstractmethod
    def handle_proto(self, message: Message):
        '''
        Process incoming ProtoBuf message.
        Subclasses should override this method.
        '''
