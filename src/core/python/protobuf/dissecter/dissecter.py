'''
dissecter - Deconstruct ProtoBuf messages into native Python types
'''


__author__ = 'Tor Slettnes'
__docformat__ = 'javadoc en'

### Standard Python modules
from collections.abc import Callable, Sequence
from dataclasses import make_dataclass
from enum import IntEnum
from typing import Container, Callable

### Third-party modules
from google.protobuf.descriptor import Descriptor, FieldDescriptor, EnumDescriptor

### Common Core modules
from cc.core.logbase import LogBase
from cc.core.stringutils import common_prefix
from cc.protobuf.wellknown import Message, MessageType
import cc.protobuf.wellknown as wellknown # Well-known types from Google
import cc.protobuf.variant   as variant   # Common Core variant type
import cc.protobuf.datetime  as datetime  # Calendar date/time types
import cc.protobuf.status    as status    # Status types

Decoder = Callable[[Message], object]


class MessageDissecter (LogBase):
    '''
    Dissect ProtoBuf messages into native Python value.

    This class may be inherited and specialized for specific interfaces,
    e.g. gRPC service clients.
    '''

    def __init__(self):
        self.decoders: dict[str, Decoder] = type(self).message_decoders.copy()

    def register_decoder(self,
                         message_type: MessageType,
                         decoder: Decoder,
                         ):
        '''
        Register a custom encoder for a ProtoBuf message type.

        @param message_type
            A ProtoBuf message type for which this decoder is registered.
            Future direct or indirect invocations of `dissect_from_value()` will
            use this decoder whenever it needs to dissect a message of this
            type.

        @param decoder
            Decoder function, which will receive a ProtoBuf message of type
            `messsage_type` as its sole input argument, and is expected to
            return a decomposed value (normally native Python object).
        '''

        self.decoders[message_type.DESCRIPTOR.full_name] = decoder


    def decode(self, message: Message) -> object:
        '''
        Decode a ProtoBuf message into a native Python value.
        '''

        try:
            decoder = self.decoders[message.DESCRIPTOR.full_name]

        except AttributeError:
            raise TypeError('%s.dissect() input must be a ProtoBuf message, got %s'
                            %(type(self).__name__, type(message).__name__)) from None

        except KeyError:
            return self.dissect_message(message)

        else:
            return decoder(message)


    def dissect_message(self, message: Message) -> object:
        '''
        Decompose a ProtoBuf message into a native Python data class.
        '''

        datafields = []
        values = []

        for fd in message.DESCRIPTOR.fields:
            if field_type := self.field_type(fd):
                datafields.append((fd.name, field_type))
            else:
                datafields.append(fd.name)

            if self.is_field_present(message, fd):
                values.append(self.dissect_field(message, fd))
            else:
                values.append(None)

        dataclass = make_dataclass(
            cls_name = message.DESCRIPTOR.name,
            fields = datafields,
        )

        return dataclass(*values)


    def dissect_field(self,
                     message: Message,
                     fd: FieldDescriptor,
                     ) -> object:

        field = getattr(message, fd.name)

        if fd.message_type and fd.message_type.GetOptions().map_entry:
            ### This is a map field. Produce a dictionary.
            key_fd, value_fd = fd.message_type.fields
            key_decoder = self.field_decoder(key_fd)
            value_decoder = self.field_decoder(value_fd)

            value = {
                key_decoder(k): value_decoder(v)
                for k, v in field.items()
            }

        else:
            decoder = self.field_decoder(fd)

            if fd.label == fd.LABEL_REPEATED:
                value = [decoder(item) for item in field]

            else:
                value = decoder(field)

        return value


    def is_field_present(self,
                         message: Message,
                         fd: FieldDescriptor) -> bool:
        if fd.containing_oneof:
            return message.WhichOneof(fd.containing_oneof.name) == fd.name
        else:
            return True


    def field_decoder(self,
                      fd: FieldDescriptor) -> Decoder:

        if enum_type := fd.enum_type:
            return self.enum_decoder(enum_type)

        elif descriptor := fd.message_type:
            return self.decode

        else:
            return lambda value: value


    def enum_decoder(self,
                     descriptor: EnumDescriptor) -> Decoder:

        try:
            return self.enum_decoders[descriptor.full_name]
        except KeyError:
            pfx_size, pfx = common_prefix(descriptor.values_by_name)

            enum_class = IntEnum(
                descriptor.name,
                {v.name[pfx_size:]: v.number for v in descriptor.values},
            )
            self.enum_decoders[descriptor.full_name] = enum_class
            return enum_class


    def field_type(self,
                   fd: FieldDescriptor) -> type:

        try:
            field_type = self.type_map[fd.type]
        except KeyError:
            if fd.message_type:
                field_type = (dict if fd.message_type.GetOptions().map_entry
                              else Container)
            elif fd.enum_type:
                field_type = IntEnum
            else:
                field_type = None


    type_map = {
        FieldDescriptor.TYPE_BOOL    : bool,
        FieldDescriptor.TYPE_FIXED32 : int,
        FieldDescriptor.TYPE_FIXED64 : int,
        FieldDescriptor.TYPE_SFIXED32: int,
        FieldDescriptor.TYPE_SFIXED64: int,
        FieldDescriptor.TYPE_INT32   : int,
        FieldDescriptor.TYPE_INT64   : int,
        FieldDescriptor.TYPE_UINT32  : int,
        FieldDescriptor.TYPE_UINT64  : int,
        FieldDescriptor.TYPE_DOUBLE  : float,
        FieldDescriptor.TYPE_FLOAT   : float,
        FieldDescriptor.TYPE_BYTES   : bytes,
        FieldDescriptor.TYPE_STRING  : str,
        FieldDescriptor.TYPE_ENUM    : IntEnum,
    }


    message_decoders = {
        'google.protobuf.Empty': lambda v: None,
        'google.protobuf.BoolValue': lambda v: v.value,
        'google.protobuf.StringValue': lambda v: v.value,
        'google.protobuf.DoubleValue': lambda v: v.value,
        'google.protobuf.FloatValue': lambda v: v.value,
        'google.protobuf.Int64Value': lambda v: v.value,
        'google.protobuf.Int32Value': lambda v: v.value,
        'google.protobuf.UInt64Value': lambda v: v.value,
        'google.protobuf.UInt32Value': lambda v: v.value,
        'google.protobuf.BytesValue': lambda v: v.value,
        'google.protobuf.Timestamp': wellknown.decodeTimestamp,
        'google.protobuf.Duration': wellknown.decodeDuration,
        'google.protobuf.Value': wellknown.decodeValue,
        'google.protobuf.Struct': wellknown.decodeStruct,
        'google.protobuf.ListValue': wellknown.decodeListValue,
        'cc.protobuf.datetime.TimeStruct': datetime.decodeTimeStruct,
        'cc.protobuf.datetime.Interval': datetime.decodeInterval,
        'cc.protobuf.variant.Value': variant.encodeValue,
        'cc.protobuf.variant.ValueList': variant.decodeValueList,
        'cc.protobuf.variant.TaggedValue': variant.decodeTaggedValue,
        'cc.protobuf.variant.TaggedValueList': variant.decodeTaggedValueList,
        'cc.protobuf.variant.KeyValueMap': variant.decodeKeyValueMap,
    }

    enum_decoders = {
        'cc.protobuf.status.Level': status.decodeLogLevel,
    }



### Generic instance
message_dissecter = MessageDissecter()

def dissecter_instance(function: Callable) -> MessageDissecter:
    try:
        instance = function.__self__
    except AttributeError:
        return message_dissecter
    else:
        if isinstance(instance, MessageDissecter):
            return instance
        else:
            return message_dissecter


def dissect_response(function: Callable) -> Callable:
    '''
    Function decorator that will dissect a ProtoBuf return value into a
    native Python object.
    '''

    def wrapper(*args, **kwargs):
        response = function(*args, **kwargs)
        dissecter = dissecter_instance(function)
        return dissecter.decode(response)

    return wrapper

def dissect_message(message: Message) -> object:
    '''
    Dissect a message using the generic 
    '''
    return message_dissecter.decode(message)
