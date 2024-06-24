#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file variant.py
## @brief Wrapper for ProtoBuf types in `variant.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

'''Utilites to encode/decode variant type containers defined in @sa
../../proto/variant.proto.

'''

### Modules wihtin package
from ..generated.variant_pb2 import Value, ValueList

### Third-party modules
from google.protobuf.timestamp_pb2 import Timestamp
from google.protobuf.duration_pb2 import Duration

def encodeValue(value : object) -> Value:
    '''Encode a Python value as a `protobuf.variant.Value` instance'''

    if isinstance(value, tuple) and len(value) == 2 and isinstance(value[0], str):
        return encodeTaggedValue(value)
    else:
        return encodeTaggedValue((None, value))

def encodeTaggedValue(pair : tuple[str, object]) -> Value:
    '''Encode a (tag, value) tuple as a `protobuf.variant.Value` instance'''

    tag, value = pair
    tv = Value(tag=tag)

    if isinstance(value, bool):
        tv.value_bool = value

    elif isinstance(value, int) and (value >= 0):
        tv.value_uint = value

    elif isinstance(value, int):
        tv.value_sint = value

    elif isinstance(value, float):
        tv.value_real = value

    elif isinstance(value, str):
        tv.value_string = value

    elif isinstance(value, bytes):
        tv.value_bytes = value

    elif isinstance(value, Timestamp):
        tv.value_timestamp.CopyFrom(value)

    elif isinstance(value, Duration):
        tv.value_duration.CopyFrom(value)

    elif isinstance(value, (list, tuple)):
        tv.value_list.items.extend([
            encodeValue(item) for item in value])

    elif isinstance(value, dict):
        tv.value_list.items.extend([
            encodeTaggedValue(item) for item in value.items()])
        tv.value_list.mappable = True

    elif value is not None:
        raise TypeError("Cannot convert type %s to protobuf value: %s"%
                        (type(value), value))
    return tv

def decodeValue(value: Value,
                autotype: bool = True) -> object:
    '''Decode a `protobuf.variant.Value` instance to a native Python value.

    The `autotype` argument recursively controls the decoded data type in case
    `value` contains a `ValueList` instance with nested values. If `True` and
    either the `ValueList.mappable` or `ValueList.untagged` field is also set,
    the resulting type becomes, respectively, a `{key: value)` dictionary or a
    list of these nested values.  Otherwise, the deocoded result becomes a list
    of `(tag, value)` pairs.

    '''

    fieldname = value.WhichOneof('value')

    if (fieldname == 'value_timestamp'):
        result = decodeTimestamp(value.value_timestamp)

    elif (fieldname == 'value_duration'):
        result = decodeDuration(value.value_duration)

    elif (fieldname == 'value_list'):
        if autotype and value.value_list.untagged:
            result = decodeValueList(value, autotype)
        elif autotype and value.value_list.mappable:
            result = decodeValueMap(value, autotyhpe)
        else:
            result = decodeTaggedValueList(value, autotype)

    elif fieldname:
        result = getattr(value, fieldname)

    else:
        result = None

    return result


def decodeTaggedValue(tv: Value, autotype=True) -> tuple[str|None, object]:
    '''Decode a `protobuf.variant.Value` instance to a (tag, value) pair.

    The `autotype` argument controls the decoding in case the value is a
    `ValueList` instance; see `decodeValue()` for details.
    '''

    tag = None

    if tv and tv.tag:
        tag = tv.tag
        value = decodeValue(tv, autotype)

    return (tag, value)

def encodeValueList(value: list|dict) -> ValueList:
    '''Encode a native Python list or dictionary to a `protobuf.variant.ValueList`.

    The input should be one of the following:
      - A {key, value} dictionary
      - A list of (tag, value) items
      - A list of values
    '''
    if value is None:
        return ValueList()

    elif isinstance(value, dict):
        return ValueList(
            items = [encodeTaggedValue(tv) for tv in value.items()],
            mappable = True)

    elif isinstance(value, list):
        return ValueList(
            items = [encodeValue(v) for v in value])

    elif isinstance(value, ValueList):
        return value

    else:
        raise ValueError("encodeToValueList() expects a dictionary or list")

def decodeTaggedValueList(valuelist: ValueList,
                          autotype=False) -> list[tuple[str|None, object]]:
    '''Decode a `protobuf.variant.ValueList` instance to a list of native
    `(tag, value)` pairs.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` items in the list; see `decodeValue()` for details.
    Note also the that the default value of this argument is different.
    '''

    if isinstance(valuelist, ValueList):
        return [decodeTaggedValue(tv, autotype) for tv in valuelist.items]
    else:
        raise ValueError("Argument must be a protobuf.variant.ValueList() instance")

def decodeValueList(valuelist, autotype=True) -> list[object]:
    '''Decode a `protobuf.variant.ValueList` instance to a list of native
    values.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` items in the list; see `decodeValue()` for details.

    '''

    return [v for (t,v) in self.decodeTaggedValueList(valuelist, autotype)]

def decodeValueMap(valuelist, autotype=True):
    '''Decode a `protobuf.variant.Value` instance to a native Python list.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` instances in the list; see `decodeValue()` for details.
    '''
    return dict(decodeTaggedValueList(valuelist, autotype))

def valueList(**kwargs):
    '''Build a `protobuf.variant.ValueList` instance from native key/value pairs.'''
    return encodeValueList(kwargs)
