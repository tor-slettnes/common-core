#!/usr/bin/echo Do not invoke directly.
#===============================================================================
## @file status.py
## @brief Wrapper module for `status.proto`
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Populate symbols from `request_reply.proto`
from ..generated.status_pb2 import *
import logging

level_map = {
    LEVEL_NONE: logging.NOTSET,
    LEVEL_TRACE: (logging.NOTSET + logging.DEBUG) // 2,
    LEVEL_DEBUG: logging.DEBUG,
    LEVEL_INFO: logging.INFO,
    LEVEL_NOTICE: (logging.INFO + logging.WARNING) // 2,
    LEVEL_WARNING: logging.WARNING,
    LEVEL_FAILED: logging.ERROR,
    LEVEL_CRITICAL: logging.CRITICAL,
    LEVEL_FATAL: logging.FATAL,
}

def decodeLogLevel(self, level: Level) -> int:
    return level_map.get(level, logging.NOTSET)

def encodeLogLevel(self, log_level: int) -> Level:
    current_event_level = LEVEL_NONE

    for candidate_event_level, candidate_log_level in level_map.items():
        if (level >= candidate_log_level) and (candidate_event_level > current_event_level):
            current_event_level = candidate_event_level

    return current_event_level
