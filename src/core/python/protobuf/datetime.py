#!/bin/echo Do not invoke directly.
#===============================================================================
## @file datetime.py
## @brief Support for ProtoBuf types from `datetime.proto`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Generated from `demo.proto`
from .utils import proto_enum
from ..generated.datetime_pb2 import TimeStruct, Weekday, Interval, TimeUnit

### Standard Python modules
import time

Weekday = proto_enum(Weekday)
TimeUnit = proto_enum(TimeUnit)

def encodeInterval(interval: tuple[int, TimeUnit] | TimeUnit | Interval) -> Interval:
    '''Convert a (COUNT, UNIT) tuple to an `Interval` message'''

    if isinstance(interval, Interval):
        return interval

    elif isinstance(interval, TimeUnit):
        return Interval(count = 1, unit = interval)

    elif (isinstance(interval, tuple) and
          len(interval) == 2 and
          isinstance(interval[1], TimeUnit)):
        return Interval(count = interval[0],
                        unit = interval[1])

    else:
        raise TypeError(
            "`interval` type must be one of: `Interval`, `TimeUnit`, or `tuple[int, TimeUnit]`")


def encodeTimeStruct(ts: time.struct_time) -> TimeStruct:
    '''Convert a `time.time_struct` object to a `TimeStruct` message.'''

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
