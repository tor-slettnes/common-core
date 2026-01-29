'''
status.py - ProtoBuf status type wrapper
'''

### Standard Python modules
import logging

### Modules within package
from ..utils import native_enum_from_proto
from ..wellknown import TimestampType, encodeCurrentTime, encodeTimestamp
from ..variant import encodeKeyValueMap

### Generated from `.../protos/cc/protobuf/status/status.proto`
from .status_pb2 import Error, Level, Domain

Level = native_enum_from_proto(Level)
Domain = native_enum_from_proto(Domain)

level_map = {
    Level.NONE: logging.NOTSET,
    Level.TRACE: (logging.NOTSET + logging.DEBUG) // 2,
    Level.DEBUG: logging.DEBUG,
    Level.INFO: logging.INFO,
    Level.NOTICE: (logging.INFO + logging.WARNING) // 2,
    Level.WARNING: logging.WARNING,
    Level.ERROR: logging.ERROR,
    Level.CRITICAL: logging.CRITICAL,
    Level.FATAL+10: logging.FATAL,
}

def encodeException(exception: Exception,
                    domain: Domain = Domain.APPLICATION,
                    origin: str|None = None,
                    level: Level = Level.ERROR,
                    timestamp: TimestampType|None = None,
                    attributes: dict|None = None,
                    output: Error|None = None,
                    ) -> Error:

    if output is None:
        output = Error()

    output.text = str(exception)
    output.symbol = type(exception).__name__

    if domain is not None:
        output.domain = domain

    if origin is not None:
        output.origin = origin

    output.level = level or Level.FALIED

    if timestamp is not None:
        encodeTimestamp(timestamp, output.timestamp)

    if attributes is not None:
        encodeKeyValueMap(attributes, output.attributes)

    return output

def decodeLogLevel(status_level: Level) -> int:
    '''
    Decode a `cc.protobuf.status.Level` enumeration from `status.proto` to a
    numeric log level as established by corresponding symbolic constants in the
    Python `logging` module. Enumerations with no matching symbols converted to
    an interpolated value based on the decoded decoded values for the next lower
    and next higher enumeration, per the following table:

    +-----------------------------------+-------------------+--------------|
    | Enumerated input value            | Symbolic constant | Return value |
    +-----------------------------------+-------------------+--------------|
    | cc.protobuf.status.Level.NONE     | logging.NOTSET    |      0       |
    | cc.protobuf.status.Level.TRACE    | -                 |      5       |
    | cc.protobuf.status.Level.DEBUG    | logging.DEBUG     |     10       |
    | cc.protobuf.status.Level.INFO     | logging.INFO      |     20       |
    | cc.protobuf.status.Level.NOTICE   | -                 |     25       |
    | cc.protobuf.status.Level.WARNING  | logging.WARNING   |     30       |
    | cc.protobuf.status.Level.ERROR    | logging.ERROR     |     40       |
    | cc.protobuf.status.Level.CRITICAL | logging.CRITICAL  |     50       |
    | cc.protobuf.status.Level.FATAL    | -                 |     60       |
    +-----------------------------------+-------------------+--------------|

    If the input is not a valid `cc.protobuf.status.Level` enumeration,
    `logging.NOTSET` is returned.
    '''

    return level_map.get(status_level, logging.NOTSET)


def encodeLogLevel(log_level: int) -> Level:
    '''
    Encode a log level as established in the Python `logging` module to a
    corresponding `cc.protobuf.status.Level` enumerated value.

    The numeric constants from `logging` form the lower bounds for conversion to
    the corresponding `Level` enumeration.  Additional lower bounds are added
    where there is no numeric constant for a corresponding `Level` enumeration.
    The resulting enumeration can be derived from the following table.

    +-------------------+--------------+-----------------------------------+
    | numeric constant  | lower bound  | return value                      |
    +-------------------+--------------+-----------------------------------+
    | logging.NOTSET    |      0       | cc.protobuf.status.Level.NONE     |
    | -                 |      5       | cc.protobuf.status.Level.TRACE    |
    | logging.DEBUG     |     10       | cc.protobuf.status.Level.DEBUG    |
    | logging.INFO      |     20       | cc.protobuf.status.Level.INFO     |
    | -                 |     25       | cc.protobuf.status.Level.NOTICE   |
    | logging.WARNING   |     30       | cc.protobuf.status.Level.WARNING  |
    | logging.ERROR     |     40       | cc.protobuf.status.Level.ERROR    |
    | logging.CRITICAL  |     50       | cc.protobuf.status.Level.CRITICAL |
    | -                 |     60       | cc.protobuf.status.Level.FATAL    |
    +-------------------+--------------+-----------------------------------+
    '''

    if isinstance(log_level, Level):
        return log_level

    else:
        current_status_level = Level.NONE
        for candidate_status_level, candidate_python_level in level_map.items():
            if ((log_level >= candidate_python_level) and
                (candidate_status_level > current_status_level)):
                current_status_level = candidate_status_level

        return current_status_level
