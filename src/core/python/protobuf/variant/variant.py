#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file variant.py
## @brief Wrapper for ProtoBuf types in `variant.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
from typing import Sequence, Mapping, Optional

### Third-party modules
from google.protobuf.timestamp_pb2 import Timestamp
from google.protobuf.duration_pb2 import Duration

### Modules wihtin package
from cc.core.timeinterval import TimeIntervalType, TimeInterval
from cc.core.timepoint import TimePointType, TimePoint

from ..wellknown import TimestampType, \
    encodeTimestamp, decodeTimestamp, encodeDuration, decodeDuration
from .variant_pb2 import Value, ValueList, \
    TaggedValue, TaggedValueList, KeyValueMap

PyValue = None|bool|int|float|str|bytes|Duration|TimestampType|list|tuple|dict
PyTaggedValue = tuple[str, PyValue]
PyValueList = list[PyValue]
PyValueDict = dict[str, PyValue]
PyTaggedValueList = list[PyTaggedValue]


def is_tagged_list(value_list: list|tuple) -> bool:
    '''Determine if the provided list is made up entirely of (tag, value) pairs'''

    for item in value_list:
        if (not isinstance(item, tuple)
            or (len(item) != 2)
            or (not instinstance(item[0], str))):
            return False
    else:
        return True


#-------------------------------------------------------------------------------
# Value

def encodeValue(input : PyValue,
                output: Value|None = None) -> Value:
    '''
    Encode a Python value as a `cc.protobuf.variant.Value` instance.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Value()

    if isinstance(input, Value):
        output.CopyFrom(input)

    elif isinstance(input, bool):
        output.value_bool = input

    elif isinstance(input, TimePointType):
        encodeTimestamp(input, output.value_timestamp)

    elif isinstance(input, TimeIntervalType):
        encodeDuration(input, output.value_duration)

    elif isinstance(input, int) and (input >= 0):
        output.value_uint = input

    elif isinstance(input, int):
        output.value_sint = input

    elif isinstance(input, float):
        output.value_real = input

    elif isinstance(input, str):
        output.value_string = input

    elif isinstance(input, bytes):
        ouptut.value_bytes = input

    elif isinstance(input, (list, tuple)):
        if is_tagged_list(input):
            encodeTaggedValueList(input, output.value_tvlist)
        else:
            encodeValueList(input, output.value_list)

    elif isinstance(input, dict):
        encodeKeyValueMap(input, output.value_kvmap)

    elif input is not None:
        raise TypeError("Cannot convert type %s to protobuf value: %s"%
                        (type(input), input))

    return output


def decodeValue(input: Value) -> PyValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.Value` instance to a native
    Python value.
    '''

    if fieldname := input.WhichOneof('value'):
        match(fieldname):
            case 'value_timestamp':
                return decodeTimestamp(input.value_timestamp)

            case 'value_duration':
                return decodeDuration(input.value_duration)

            case 'value_list':
                return decodeValueList(input.value_list)

            case 'value_tvlist':
                return decodeTaggedValueList(input.value_tvlist)

            case 'value_kvmap':
                return decodeKeyValueMap(input.value_kvmap)

            case _:
                return getattr(input, fieldname)

    else:
        return None


#-------------------------------------------------------------------------------
# ValueList

def encodeValueList(input: PyValueList,
                    output: ValueList|None = None) -> ValueList:
    '''
    Encode a native Python list to a ProtoBuf
    `cc.protobuf.variant.ValueList` instance.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = ValueList()

    if isinstance(input, ValueList):
        output.CopyFrom(input)

    elif isinstance(input, list):
        output.items.extend([encodeValue(v) for v in input])

    elif input is not None:
        raise TypeError("encodeToValueList() expects a dictionary or list")

    return output


def decodeValueList(input: ValueList) -> PyValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.ValueList` instance to a list of
    native values.
    '''

    return [decodeValue(v) for v in input.items]


#-------------------------------------------------------------------------------
# TaggedValue

def encodeTaggedValue(input : PyTaggedValue,
                      output: TaggedValue|None = None) -> TaggedValue:
    '''
    Encode a (tag, value) tuple as a `cc.protobuf.variant.TaggedValue`
    instance.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = TaggedValue()

    if isinstance(input, TaggedValue):
        output.CopyFrom(input)

    else:
        tag, value = input
        output.tag = tag
        encodeValue(value, output.value)

    return output


def decodeTaggedValue(input: TaggedValue) -> PyTaggedValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.TaggedValue` instance to a Python
    (tag, value) tuple.
    '''

    return (input.tag, decodeValue(input.value))


#-------------------------------------------------------------------------------
# TaggedValueList

def encodeTaggedValueList(input: PyTaggedValueList|PyValueDict,
                          output: TaggedValueList|None = None) -> TaggedValueList:
    '''
    Encode a Python-native list of (tag, value) items to a
    `cc.protobuf.variant.TaggedValueList` instance.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = TaggedValueList()

    if isinstance(input, TaggedvalueList):
        output.CopyFrom(input)

    elif isinstance(input, list):
        output.items.extend([encodeTaggedValue(tv) for tv in input])

    elif isinstance(input, dict):
        output.items.extend([encodeTaggedValue(tv) for tv in input.items()])

    elif input is not None:
        raise TypeError("encodeTaggedValueList() expects a dictionary or list")

    return output


def decodeTaggedValueList(input: TaggedValueList|Sequence[Value]) -> PyTaggedValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.TaggedValueList` instance to a
    list of native `(tag, value)` pairs.
    '''

    if isinstance(input, TaggedValueList):
        return [decodeTaggedValue(tv) for tv in input.items]
    elif isinstance(input, Sequence):
        return [decodeTaggedValue(tv) for tv in input]
    else:
        raise TypeError(
            "Argument must be a protobuf.variant.ValueList() instance "
            "or a native list, not %s"%(type(input),))


#-------------------------------------------------------------------------------
# KeyValueMap

def encodeKeyValueMap(input: PyValueDict,
                      output: KeyValueMap|None = None) -> KeyValueMap:
    '''
    Encode a `protobuf.variant.KeyValueMap` instance to a native Python
    dictionary.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = KeyValueMap()

    if isinstance(input, KeyValueMap):
        output.CopyFrom(inpute)

    elif isinstance(input, Mapping):
        for (key, value) in input.items():
            encodeValue(value, output.map[key])

    elif input is not None:
        raise TypeError(
            f'encodeKeyValueMap() expects a mappable input, '
            f'got {type(input).__name__}: {input}'
        )

    return output


def decodeKeyValueMap(input: KeyValueMap) -> PyValueDict:
    '''
    Decode a `protobuf.variant.KeyValueMap` instance to a native Python dictionary.
    '''
    return {key:decodeValue(value) for (key, value) in input.map.items()}


def keyValueMap(**kwargs) -> KeyValueMap:
    '''
    Build a `protobuf.variant.KeyValueMap` instance from native key/value
    pairs.
    '''
    return encodeKeyValueMap(kwargs)
