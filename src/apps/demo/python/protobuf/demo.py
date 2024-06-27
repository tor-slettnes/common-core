#!/bin/echo Do not invoke directly.
#===============================================================================
## @file demo.py
## @brief Support for ProtoBuf types from `demo.proto`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Generated from `demo.proto`
from ..generated.demo_pb2 import Greeting, TimeStruct, TimeData, Weekday, Signal

### Standard Python modules
import time, enum

def encodeTimeStruct(ts: time.struct_time) -> TimeStruct:
    '''Convert a `time.time_struct` object to a `protobuf.demo.TimeStruct` message.'''

    return TimeStruct(
        year = ts.tm_year,
        month = ts.tm_mon,
        day = ts.tm_mday,
        hour = ts.tm_hour,
        minute = ts.tm_min,
        second = ts.tm_sec,
        weekday = (ts.tm_wday+1)%7,
        year_day = ts.tm_yday,
        is_dst = ts.tm_isdst,
    )

def decodeTimeStruct (ts: TimeStruct) -> time.struct_time:
    '''Convert a `Timestruct` message to a `time.time_struct` object'''

    return time.time_struct((
        ts.year, ts.month, ts.day,
        ts.hour, ts.minute, ts.second,
        (ts.weekday-1)%7, ts.year_day, ts.is_dst))
