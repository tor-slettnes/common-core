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
from google.protobuf.duration_pb2 import Duration
from google.protobuf.timestamp_pb2 import Timestamp
from google.protobuf.wrappers_pb2 \
    import BoolValue, StringValue, DoubleValue, FloatValue, \
    Int64Value, UInt64Value, Int32Value, UInt32Value, \
    BytesValue
from google.protobuf.struct_pb2 import Value, ListValue, Struct

### Standard Python modules
from time import struct_time, mktime
from datetime import datetime

### Type type hint for timestamps
TimestampType = Timestamp|float|int|str|struct_time|datetime
DurationType = Duration|float|int
MessageType = type(Empty)

### Static values
empty = Empty()

def decodeTimestamp(prototime: Timestamp) -> float:
    '''
    Convert a ProtoBuf `Timestamp` value as an Python timestamp (seconds
    since UNIX epoch).
    '''
    return prototime.seconds + prototime.nanos*1e-9

def encodeTimestamp(timestamp: TimestampType) -> Timestamp:
    '''
    Convert the specified time to a ProtoBuf `Timestamp` value.

    The input may be one of the following:
      - None, in which case the current system time is used
      - An integer or floating point instance representing seconds since UNIX epoch
      - A JavaScript time string: "_yyyy_`-`_mm_`-`_dd_`T`_hh_`:`_mm_`:`_ss_`Z`"
        (note that the trailing `Z` implies zero offset from UTC).
      - A `time.struct_time` instance, as returned from `time.localtime()`
      - A `datetime.datetime` instance, as returned from `datetime.datetime.now()`
      - A ProtoBuf `Timestamp` value (no conversion done)

    The return value is a new Timestamp() instance.
    '''

    ts = Timestamp()

    if isinstance(timestamp, Timestamp):
        ts.CopyFrom(timestamp)

    elif isinstance(timestamp, int):
        ts.seconds = timestamp

    elif isinstance(timestamp, float):
        ts.seconds = int(timestamp)
        ts.nanos = int((timestamp-int(timestamp))*1e9)

    elif isinstance(timestamp, str):
        ts.FromJsonString(timestamp)

    elif isinstance(timestamp, struct_time):
        ts.seconds = int(mktime(timestamp))

    elif isinstance(timestamp, datetime):
        ts.FromDatetime(timestamp)

    elif timestamp is not None:
        raise TypeError("Cannot encode ProtoBuf timestamp from %s"%(type(timestamp).__name__,))

    return ts


def decodeDuration(duration: Duration) -> float:
    '''
    Decode a ProtoBuf `Duration` value to number of seconds
    '''
    return duration.seconds + duration.nanos*1e-9


def encodeDuration(duration: int|float) -> Duration:
    '''
    Encode a number of seconds as `Duration` value
    '''

    if not duration:
        duration = 0

    return Duration(seconds=int(duration),
                    nanos=int((duration-int(duration))*1e9))


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
    kind = v.WhichOneof('kind')
    if kind == 'null_value':
        return None
    elif kind == 'bool_value':
        return value.bool_value
    elif kind == 'number_value':
        if value.number_value.is_integer():
            return int(value.number_value)
        else:
            return value.number_value
    elif kind == 'string_value':
        return value.string_value
    elif kind == 'struct_value':
        return decodeStruct(value.struct_value)
    elif kind == 'list_value':
        return decodeListValue(value.list_value)
    else:
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


