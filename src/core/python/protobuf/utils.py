#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file utils.py
## @brief Utility functions for ProtoBuf messages
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules within package
from .wellknown import MessageType, MessageToString, MessageToDict, ParseDict

### Third-party modules
from google.protobuf.message \
    import Message, Error, EncodeError, DecodeError

from google.protobuf.internal.enum_type_wrapper \
    import EnumTypeWrapper

### Standard Python modules
from typing import Optional, Callable, Mapping, Union
import enum

#===============================================================================
# Methods

def check_type(value: Message,
               expected: type = Message,
               argument: str|None = None):
    '''Ensure that a provided value is an of the specified ProtoBuf message
    type. If not, a TypeError is raised.

    This will typically be useful within a function body to validate its
    input parameters.

    The `expected` argument should be a ProtoBuf message type generated with
    `protoc`.
    '''

    assert(isinstance(expected, MessageType) and issubclass(expected, Message))
    if not isinstance(value, expected):
        raise TypeError('%sExpected %s, not %s'%(
            'Argument %r: '%(argument,) if argument else '',
            expected.DESCRIPTOR.full_name,
            type(value).__name__)) from None


def messageToDict(message : Message,
                  use_integers_for_enums : bool = True,
                  including_default_value_fields : bool = False):
    '''
    Convert a ProtoBuf message to a Python Dictionary.
    '''

    return MessageToDict(
        message,
        including_default_value_fields=including_default_value_fields,
        use_integers_for_enums=use_integers_for_enums,
        preserving_proto_field_name=True)


def dictToMessage(value     : dict,
                  prototype : MessageType,
                  ignore_unknown_fields : bool = False) -> Message:
    '''Convert a specially constructed Python dictionary to a ProtoBuf message
    of a given type.  Refer to `ParseDict` for details.
    '''

    check_type(prototype)
    msg = prototype()
    ParseDict(
        value,
        msg,
        ignore_unknown_fields = ignore_unknown_fields)
    return msg


def valueToEnum(value: int|enum.Enum,
                enumeration: enum.EnumType) -> enum.Enum:
    '''Convert an integer value to a Python enumerated value'''

    if isinstance(value, enumeration):
        return value
    elif isinstance(value, int):
        return enumeration(value)
    else:
        raise TypeError('Value must be an integer or enumeration')


def enumToValue(value: int|enum.Enum) -> int:
    if isinstance(value, enum.Enum):
        return value.value
    elif isinstance(value, int):
        return value
    else:
        raise TypeError('Value must be an integer or enumeration')


def print_message(message: Message, as_one_line=True):
    '''Convenience wrapper to print a protobuf message.'''

    print(MessageToString(message, as_one_line=as_one_line))


def print_dict(message: Message):
    '''Convenience wrapper to print a protobuf message as a dictionary.'''

    print(MessageToDict(message))
