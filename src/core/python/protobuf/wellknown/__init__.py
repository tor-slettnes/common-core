'''
__init__.py - Collection of well-known ProtoBuf data types
'''

__author__ = 'Tor Slettnes'

### Symbols from `google.protobuf` package
from google.protobuf.text_format import MessageToString
from google.protobuf.json_format import MessageToDict
from google.protobuf.json_format import ParseDict

from google.protobuf.message import Message, Error, EncodeError, DecodeError

from google.protobuf.empty_pb2 import Empty
from google.protobuf.duration_pb2 import Duration
from google.protobuf.timestamp_pb2 import Timestamp

from google.protobuf.wrappers_pb2 import (
    BoolValue, StringValue, DoubleValue, FloatValue,
    Int64Value, UInt64Value, Int32Value, UInt32Value,
    BytesValue,
)

from google.protobuf.struct_pb2 import (
    Value, ListValue, Struct, NULL_VALUE
)

from .wellknown import (
    TimestampType, DurationType,
    encodeTimestamp, decodeTimestamp,
    encodeDuration, decodeDuration,
    encodeValue, decodeValue,
    encodeStruct, decodeStruct,
    encodeListValue, decodeListValue,
)

MessageType = type(Empty)

### Static values
empty = Empty()

