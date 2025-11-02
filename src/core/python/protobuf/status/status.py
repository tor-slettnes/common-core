#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file status.py
## @brief Wrapper module for `status.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

from ..utils import proto_enum
from .status_pb2 import Error, Level, Domain

import logging

Level = proto_enum(Level)
Domain = proto_enum(Domain)

level_map = {
    Level.NONE: logging.NOTSET,
    Level.TRACE: (logging.NOTSET + logging.DEBUG) // 2,
    Level.DEBUG: logging.DEBUG,
    Level.INFO: logging.INFO,
    Level.NOTICE: (logging.INFO + logging.WARNING) // 2,
    Level.WARNING: logging.WARNING,
    Level.ERROR: logging.ERROR,
    Level.CRITICAL: logging.CRITICAL,
    Level.FATAL: logging.FATAL,
}

def decodeLogLevel(event_level: Level) -> int:
    '''
    Decode a `cc.protobuf.status.Level` enumeration from `status.proto` to a
    log level as established in the Python `logging` module.

    If the input is not a valid `cc.protobuf.status.Level` enumeration, `None`
    is returned.
    '''

    return level_map.get(event_level)


def encodeLogLevel(log_level: int) -> Level:
    '''
    Encode a log level as established in the Python `logging` module to an
    enumerated `cc.protobuf.status.Level` value from `status.proto`.
    '''

    if isinstance(log_level, Level):
        return log_level

    else:
        current_event_level = Level.NONE
        for candidate_event_level, candidate_log_level in level_map.items():
            if ((log_level >= candidate_log_level) and
                (candidate_event_level > current_event_level)):
                current_event_level = candidate_event_level

        return current_event_level
