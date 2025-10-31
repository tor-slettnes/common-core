#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file wellknown.py
## @brief Conversions between native Python types and well-known ProtoBuf types
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Standard Python modules
from typing import Sequence, Mapping, Optional
from time import struct_time, mktime
from datetime import datetime


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
from google.protobuf.struct_pb2 import Value, ListValue, Struct, NULL_VALUE

### Modules within this package
from cc.core.timeinterval import TimeIntervalType, TimeInterval
from cc.core.timepoint import TimePointType, TimePoint

### Type type hint for timestamps
MessageType = type(Empty)
TimestampType = TimePointType|Timestamp|int|float
DurationType = TimeIntervalType|Duration|int|float

### Static values
empty = Empty()

def decodeTimestamp(prototime: Timestamp) -> TimePoint:
    '''
    Decode a ProtoBuf Timestamp input into a Python-compatible timestamp
    (`float` representing seconds since Epoch).
    '''
    return TimePoint.from_protobuf(prototime)


def encodeTimestamp(input: TimestampType,
                    output: Timestamp|None = None) -> Timestamp:
    '''
    Convert an existing timestamp to a ProtoBuf `Timestamp` value.

    The input may be any value accepted by
    `cc.core.timepoint.TimePoint.from_value()`, including:

      - An existing `google.protobuf.Timestamp` value
      - An existing `cc.core.timepoint.TimePoint` value
      - An `int`, `float`, or numeric string representing seconds since UNIX epoch
      - An ISO 8601 formatted string, preferably with a `Z` suffix to indicate UTC
      - A `time.struct_time` instance, as returned from `time.localtime()`
      - A `datetime.datetime` instance, as returned from `datetime.datetime.now()`

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Timestamp()

    tp = TimePoint.from_value(input)
    output.seconds = int(tp)
    output.nanos   = int((tp - int(tp)) * 1e9)
    return output


def decodeDuration(duration: Duration) -> TimeInterval:
    '''
    Decode a ProtoBuf Timestamp input into a Python-compatible time interval
    (`float` representing seconds).
    '''
    return TimeInterval.from_protobuf(duration)


def encodeDuration(input: TimeIntervalType,
                   output: Duration|None = None) -> Duration:
    '''
    Encode an existing duration to a ProtoBuf `Duration` value.

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

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Duration()

    dur = TimeInterval.from_value(input or 0)
    output.seconds = int(dur)
    output.nanos   = int((dur - int(dur)) * 1e9)
    return output


def encodeValue(input: object,
                output: Value|None = None) -> Value:
    '''
    Encode a simple Python value as a `google.protobuf.Value` variant

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Value()

    if isinstance(input, Value):
        output.CopyFrom(input)

    elif input is None:
        output.null_value = NULL_VALUE

    elif isinstance(input, bool):
        output.bool_value = input

    elif isinstance(input, (int, float)):
        output.number_value = input

    elif isinstance(input, str):
        output.string_value = input

    elif isinstance(input, dict):
        encodeStruct(input, output.struct_value)

    elif isinstance(input, list):
        encodeList(input, output.list_value)

    else:
        raise TypeError("Unable to encode value type %r as protobuf.Value"%
                         (type(input).__name__))
    return output


def encodeStruct(input: dict,
                 output: Struct|None = None) -> Struct:
    '''
    Encode a Python dictionary as a `google.protobuf.Struct` instance

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Struct()

    if isinstance(input, Struct):
        output.CopyFrom(input)

    elif isinstance(input, Mapping):
        for (key, value) in input.items():
            encodeValue(value, output.fields[key])

    elif input is not None:
        raise TypeError(
            f'encodeStruct() expects a mappable input, '
            f'got {type(input).__name__}: {input}'
        )

    return output


def encodeListValue(input: list,
                    output: ListValue|None = None) -> ListValue:
    '''
    Encode a Python list as a `google.protobuf.ListValue` instance

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''
    if output is None:
        output = ListValue()

    if isinstance(input, ListValue):
        output.CopyFrom(input)

    elif isinstance(input, Sequence):
        output.values.extend([encodeValue(value) for value in input])

    elif input is not None:
        raise TypeError(
            f'encodeListValue() expects a sequence type as input, '
            f'got {type(input).__name__}: {input}'
        )

    return output


def decodeValue(value: Value) -> object:
    '''
    Decode a `google.protobuf.Value` variant to a native Python value
    '''

    match(value.WhichOneof('kind')):
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


def decodeStruct(struct: Struct) -> dict:
    '''
    Decode a `google.protobuf.Struct` instance to a native Python dictionary
    '''
    return dict([(key, decodeValue(value))
                 for (key, value) in struct.fields.items()])

def decodeListValue(listvalue: ListValue) -> list:
    '''
    Decode a `google.protobuf.ListValue` instance to a native Python list
    '''
    return [ decodeValue(value)
             for value in listvalue.values ]


