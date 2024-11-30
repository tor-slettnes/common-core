#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file variant.py
## @brief Wrapper for ProtoBuf types in `variant.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Modules wihtin package
from cc.generated.variant_pb2 import Value, ValueList, Complex
from .wellknown import decodeTimestamp, decodeDuration

### Third-party modules
from google.protobuf.timestamp_pb2 import Timestamp
from google.protobuf.duration_pb2 import Duration

### Standard Python modules
from typing import Sequence, Optional

PyValue = object
PyTaggedValue = tuple[str, object]
PyValueList = list[object]
PyValueDict = dict[str, object]
PyTaggedValueList = list[PyTaggedValue]

PyOptTaggedValue = tuple[str|None, object]
PyOptTaggedValueList = list[PyOptTaggedValue]

def encodeComplex(value: complex) -> Complex:
    return Complex(real = value.real, imag = value.imag)

def decodeComplex(value: Complex) -> complex:
    return complex(value.real, value.imag)


def encodeValue(value : PyTaggedValue|PyValue) -> Value:
    '''Encode a Python value as a `protobuf.variant.Value` instance'''

    if isinstance(value, tuple) and len(value) == 2 and isinstance(value[0], str):
        return encodeTaggedValue(value)
    else:
        return encodeTaggedValue((None, value))


def decodeValue(value: Value,
                autotype: bool = True) -> PyValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.Value` instance to a native Python
    value.

    The `autotype` argument recursively controls the decoded data type in case
    `value` contains a `ValueList` instance with nested values. If `True` and
    either the `ValueList.mappable` or `ValueList.untagged` field is also set,
    the resulting type becomes, respectively, a `{key: value)` dictionary or a
    list of these nested values.  Otherwise, the deocoded result becomes a list
    of `(tag, value)` pairs.
    '''

    if fieldname := value.WhichOneof('value'):
        fieldvalue = getattr(value, fieldname)

        if isinstance(fieldvalue, Timestamp):
            return decodeTimestamp(fieldvalue)

        elif isinstance(fieldvalue, Duration):
            return decodeDuration(fieldvalue)

        elif isinstance(fieldvalue, Complex):
            return decodeComplex(fieldvalue)

        elif isinstance(fieldvalue, ValueList):
            if autotype and value.value_list.untagged:
                return decodeValueList(value, autotype)
            elif autotype and value.value_list.mappable:
                return decodeValueMap(value, autotyhpe)
            else:
                return decodeTaggedValueList(value, autotype)
        else:
            return fieldvalue

    else:
        return None


def encodeTaggedValue(pair : PyOptTaggedValue) -> Value:
    '''
    Encode a (tag, value) tuple as a `protobuf.variant.Value` instance
    '''

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

    elif isinstance(value, complex):
        tv.value_complex.CopyFrom(encodeComplex(value))

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


def decodeTaggedValue(tv: Value, autotype=True) -> PyOptTaggedValue:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.Value` instance to a (tag, value)
    pair.

    The `autotype` argument controls the decoding in case the value is a
    `ValueList` instance; see `decodeValue()` for details.
    '''

    tag = None

    if tv and tv.tag:
        tag = tv.tag
        value = decodeValue(tv, autotype)

    return (tag, value)

def encodeValueList(value: PyValueList|PyTaggedValueList|PyValueDict) -> ValueList:
    '''
    Encode a native Python list or dictionary to a ProtoBuf
    `cc.protobuf.variant.ValueList` instance.

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

def decodeValueList(valuelist, autotype=True) -> PyValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.ValueList` instance to a list of
    native values.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` items in the list; see `decodeValue()` for details.
    '''

    return [v for (t,v) in self.decodeTaggedValueList(valuelist, autotype)]

def encodeTaggedValueList(valuelist: PyOptTaggedValueList) -> ValueList:
    '''
    Encode a Python-native list of (tag, value) items to a
    `cc.protobuf.variant.ValueList` instance.
    '''

    if isinstance(valuelist, list):
        return ValueList(items = [encodeTaggedValue(pair) for pair in valuelist])

    elif isinstance(valuelist, ValueList):
        return valuelist

    else:
        raise ValueError("encodeTaggedValueList() expects a dictionary or list")


def decodeTaggedValueList(valuelist: ValueList|Sequence[Value],
                          autotype=False) -> PyOptTaggedValueList:
    '''
    Decode a ProtoBuf `cc.protobuf.variant.ValueList` instance to a list of
    native `(tag, value)` pairs.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` items in the list; see `decodeValue()` for details.
    Note also the that the default value of this argument is different.
    '''

    if isinstance(valuelist, ValueList):
        return [decodeTaggedValue(tv, autotype) for tv in valuelist.items]
    elif isinstance(valuelist, Sequence):
        return [decodeTaggedValue(tv, autotype) for tv in valuelist]
    else:
        raise ValueError(
            "Argument must be a protobuf.variant.ValueList() instance "
            "or a native list, not %s"%(type(valuelist),))

def decodeValueMap(valuelist, autotype=True) -> PyValueDict:
    '''
    Decode a `protobuf.variant.Value` instance to a native Python list.

    The `autotype` argument recursively controls the decoding of any nested
    `ValueList` instances in the list; see `decodeValue()` for details.
    '''
    return dict(decodeTaggedValueList(valuelist, autotype))

def valueList(**kwargs) -> ValueList:
    '''
    Build a `protobuf.variant.ValueList` instance from native key/value
    pairs.
    '''
    return encodeValueList(kwargs)
