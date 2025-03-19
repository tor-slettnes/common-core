#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file variant.py
## @brief Wrapper for ProtoBuf types in `variant.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules wihtin package
from cc.generated.variant_pb2 import Value, ValueList, \
    TaggedValue, TaggedValueList, KeyValueMap
from .wellknown import TimestampType, \
    encodeTimestamp, decodeTimestamp, decodeDuration

### Third-party modules
from google.protobuf.timestamp_pb2 import Timestamp
from google.protobuf.duration_pb2 import Duration

### Standard Python modules
from typing import Sequence, Mapping, Optional

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

def encodeValue(value : PyValue) -> Value:
    '''Encode a Python value as a `cc.protobuf.variant.Value` instance'''

    if isinstance(value, bool):
        return Value(value_bool = value)

    elif isinstance(value, int) and (value >= 0):
        return Value(value_uint = value)

    elif isinstance(value, int):
        return Value(value_sint = value)

    elif isinstance(value, float):
        return Value(value_real = value)

    elif isinstance(value, str):
        return Value(value_string = value)

    elif isinstance(value, bytes):
        return Value(value_bytes = value)

    elif isinstance(value, Duration):
        return Value(value_duration = value)

    elif isinstance(value, Timestamp):
        return Value(value_timestamp = value)

    elif isinstance(value, TimestampType):
        return Value(value_timestamp = encodeTimestamp(value))

    elif isinstance(value, (list, tuple)):
        if is_tagged_list(value):
            return Value(tv_list = encodeTaggedValueList(value))
        else:
            return Value(value_list = encodeValueList(value))

    elif isinstance(value, dict):
        return Value(kv_map = encodeKeyValueMap(value))

    elif value is None:
        return Value()

    else:
        raise TypeError("Cannot convert type %s to protobuf value: %s"%
                        (type(value), value))


def decodeValue(value: Value) -> PyValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.Value` instance to a native
    Python value.
    '''

    if fieldname := value.WhichOneof('value'):
        fieldvalue = getattr(value, fieldname)

        if isinstance(fieldvalue, Timestamp):
            return decodeTimestamp(fieldvalue)

        elif isinstance(fieldvalue, Duration):
            return decodeDuration(fieldvalue)

        elif isinstance(fieldvalue, ValueList):
            return decodeValueList(fieldvalue)

        elif isinstance(fieldvalue, TaggedValueList):
            return decodeTaggedValueList(fieldvalue)

        elif isinstance(fieldvalue, KeyValueMap):
            return decodeKeyValueMap(fieldvalue)

        else:
            return fieldvalue
    else:
        return None


#-------------------------------------------------------------------------------
# ValueList

def encodeValueList(value: PyValueList) -> ValueList:
    '''
    Encode a native Python list to a ProtoBuf
    `cc.protobuf.variant.ValueList` instance.
    '''

    if value is None:
        return ValueList()

    elif isinstance(value, list):
        return ValueList(
            items = [encodeValue(v) for v in value])

    elif isinstance(value, ValueList):
        return value

    else:
        raise ValueError("encodeToValueList() expects a dictionary or list")

def decodeValueList(valuelist: ValueList) -> PyValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.ValueList` instance to a list of
    native values.
    '''

    return [decodeValue(v) for v in valuelist.items]


#-------------------------------------------------------------------------------
# TaggedValue

def encodeTaggedValue(pair : PyTaggedValue) -> Value:
    '''
    Encode a (tag, value) tuple as a `cc.protobuf.variant.TaggedValue`
    instance
    '''

    tag, value = pair
    return TaggedValue(tag = tag, value = encodeValue(value))


def decodeTaggedValue(tv: TaggedValue) -> PyTaggedValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.TaggedValue` instance to a Python
    (tag, value) tuple.
    '''

    return (tv.tag, decodeValue(tv.value))


#-------------------------------------------------------------------------------
# TaggedValueList

def encodeTaggedValueList(tvlist: PyTaggedValueList|PyValueDict) -> TaggedValueList:
    '''
    Encode a Python-native list of (tag, value) items to a
    `cc.protobuf.variant.TaggedValueList` instance.
    '''

    if isinstance(tvlist, list):
        return TaggedValueList(items = [encodeTaggedValue(tv) for tv in tvlist])

    elif isinstance(tvlist, dict):
        return TaggedValueList(items = [encodeTaggedValue(tv) for tv in tvlist.items()])

    elif isinstance(tvlist, TaggedValueList):
        return tvlist

    else:
        raise ValueError("encodeTaggedValueList() expects a dictionary or list")


def decodeTaggedValueList(tvlist: TaggedValueList|Sequence[Value]) -> PyTaggedValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.TaggedValueList` instance to a
    list of native `(tag, value)` pairs.
    '''

    if isinstance(tvlist, TaggedValueList):
        return [decodeTaggedValue(tv) for tv in tvlist.items]
    elif isinstance(tvlist, Sequence):
        return [decodeTaggedValue(tv) for tv in tvlist]
    else:
        raise ValueError(
            "Argument must be a protobuf.variant.ValueList() instance "
            "or a native list, not %s"%(type(valuelist),))


#-------------------------------------------------------------------------------
# KeyValueMap

def encodeKeyValueMap(kvmap: PyValueDict) -> KeyValueMap:
    '''
    Encode a `protobuf.variant.KeyValueMap` instance to a native Python dictionary.
    '''
    return KeyValueMap(map = {key:encodeValue(value) for (key, value) in kvmap.items()})


def decodeKeyValueMap(kvmap: KeyValueMap) -> PyValueDict:
    '''
    Decode a `protobuf.variant.KeyValueMap` instance to a native Python dictionary.
    '''

    return {key:decodeValue(value) for (key, value) in kvmap.map.items()}


def keyValueMap(**kwargs) -> KeyValueMap:
    '''
    Build a `protobuf.variant.KeyValueMap` instance from native key/value
    pairs.
    '''
    return encodeKeyValueMap(kwargs)
