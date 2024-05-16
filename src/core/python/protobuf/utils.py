#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file utils.py
## @brief Utility functions for ProtoBuf messages
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules relative to current folder
from .wellknown import MessageToString, MessageToDict, ParseDict, Message, MessageType

### Standard Python modules
from typing import Optional, Callable, Mapping, Union

#===============================================================================
# Methods

def check_type(value: Message,
               expected: MessageType = Message):
    '''Ensure that a provided value is an of the specified ProtoBuf message
    type. If not, a TypeError is raised.

    This will typically be useful within a function body to validate its
    input parameters.

    The `expected` argument should be a ProtoBuf message type generated with
    `protoc`.
    '''

    assert (expected is Message) or isinstance(expected, MessageType)
    if not isinstance(value, expected):
        raise TypeError('Expected %s, not %s'%
                        (expected.DESCRIPTOR.full_name, type(value).__name__)) from None


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

    msg = prototype()
    ParseDict(
        value,
        msg,
        ignore_unknown_fields = ignore_unknown_fields)
    return msg


def print_message(message: Message, as_one_line=True):
    '''Convenience wrapper to print a protobuf message.'''

    print(MessageToString(message, as_one_line=as_one_line))


def print_dict(message: Message):
    '''Convenience wrapper to print a protobuf message as a dictionary.'''

    print(MessageToDict(message))
