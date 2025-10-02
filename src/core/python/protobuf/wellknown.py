#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file wellknown.py
## @brief Conversions between native Python types and well-known ProtoBuf types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Symbols from `google.protobuf` package
from google.protobuf.text_format import MessageToString
from google.protobuf.json_format import MessageToDict
from google.protobuf.json_format import ParseDict

from google.protobuf.message \
    import Message, Error, EncodeError, DecodeError

from google.protobuf.internal.enum_type_wrapper \
    import EnumTypeWrapper

from google.protobuf.empty_pb2 import Empty
from google.protobuf.duration_pb2 import Duration as Duration
from google.protobuf.timestamp_pb2 import Timestamp as Timestamp
from google.protobuf.wrappers_pb2 \
    import BoolValue, StringValue, DoubleValue, FloatValue, \
    Int64Value, UInt64Value, Int32Value, UInt32Value, \
    BytesValue
from google.protobuf.struct_pb2 import Value, ListValue, Struct

### Standard Python modules
from time import struct_time, mktime
from datetime import datetime

### Modules within this package
from cc.core.timeinterval import TimeIntervalType, TimeInterval
from cc.core.timepoint import TimePointType, TimePoint

### Type type hint for timestamps
MessageType = type(Empty)

### Static values
empty = Empty()

def decodeTimestamp(prototime: Timestamp) -> TimePoint:
    '''
    Decode a ProtoBuf Timestamp input into a Python-compatible timestamp
    (`float` representing seconds since Epoch).
    '''
    return TimePoint.from_protobuf(prototime)


def encodeTimestamp(timestamp: TimePointType) -> Timestamp:
    '''
    Convert an existing timestamp to a new ProtoBuf `Timestamp` value.

    The input may be any value accepted by
    `cc.core.timepoint.TimePoint.from_value()`, including:

      - An existing `google.protobuf.Timestamp` value
      - An existing `cc.core.timepoint.TimePoint` value
      - An `int`, `float`, or numeric string representing seconds since UNIX epoch
      - An ISO 8601 formatted string, preferably with a `Z` suffix to indicate UTC
      - A `time.struct_time` instance, as returned from `time.localtime()`
      - A `datetime.datetime` instance, as returned from `datetime.datetime.now()`

    The return value is a new `google.protobuf.Timestamp()` instance.
    '''
    return TimePoint.from_value(timestamp).to_protobuf()


def decodeDuration(duration: Duration) -> TimeInterval:
    '''
    Decode a ProtoBuf Timestamp input into a Python-compatible time interval
    (`float` representing seconds).
    '''
    return TimeInterval.from_protobuf(duration)


def encodeDuration(duration: TimeIntervalType) -> Duration:
    '''
    Encode an existing duration to a new `Duration` value.

    The input may be any value accepted by
    `cc.core.timeinterval.TimeInterval.from_value()`, including:

     - An existing `google.protobuf.Duration` value
     - An existing `cc.core.timeinterval.TimeInterval` value
     - An `int`, `float`, or numeric string representing seconds
     - An existing `datetime.timedelta` instance
     - An annotated string, e.g, as returned by
        * `google.protobuf.Duration().ToJsonString()`
        * `cc.core.timeinterval.TimeInterval().to_json_string()`
        * `cc.core.timeinterval.TimeInterval().to_string()`
    '''
    return TimeInterval.from_value(duration or 0).to_protobuf()


def encodeValue (value) -> Value:
    '''
    Encode a simple Python value as a `google.protobuf.Value` variant
    '''

    if value is None:
        return Value(null_value=protobuf.NullValue())

    elif isinstance(value, bool):
        return Value(bool_value=value)

    elif isinstance(value, (int, float)):
        return Value(number_value=value)

    elif isinstance(value, str):
        return Value(string_value=value)

    elif isinstance(value, dict):
        return Value(struct_value=encodeProtoStruct(value))

    elif isinstance(value, list):
        return Value(list_value=encodeProtoList(value))

    else:
        raise ValueError("Unable to encode value type %r as protobuf.Value"%
                         (type(value).__name__))


def encodeStruct (dictionary: dict) -> Struct:
    '''
    Encode a Python dictionary as a `google.protobuf.Struct` instance
    '''

    items = [(key, encodeProtoValue(value))
             for (key, value) in dictionary.items() ]
    return Struct(fields=dict(items))


def encodeListValue (listvalue: list) -> ListValue:
    '''
    Encode a Python list as a `google.protobuf.ListValue` instance
    '''
    values = [encodeProtoValue(value) for value in listvalue]
    return ListValue(values=values)


def decodeValue (value: Value) -> object:
    '''
    Decode a `google.protobuf.Value` variant to a native Python value
    '''

    match(v.WhichOneof('kind')):
        case 'null_value':
            return None

        case 'bool_value':
            return value.bool_value

        case 'number_value':
            if value.number_value.is_integer():
                return int(value.number_value)
            else:
                return value.number_value

        case 'string_value':
            return value.string_value

        case 'struct_value':
            return decodeStruct(value.struct_value)


        case 'list_value':
            return decodeListValue(value.list_value)

        case _:
            return None


def decodeStruct(structvalue: Struct) -> dict:
    '''
    Decode a `google.protobuf.Struct` instance to a native Python dictionary
    '''
    return dict([(key, decodeValue(value))
                 for (key, value) in structvalue.fields.items()])

def decodeListValue(listvalue: ListValue) -> list:
    '''
    Decode a `google.protobuf.ListValue` instance to a native Python list
    '''
    return [ decodeValue(value)
             for value in listvalue.values ]


