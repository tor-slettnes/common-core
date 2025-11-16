#!/bin/echo Do not invoke directly.
#===============================================================================
## @file datetime.py
## @brief Support for ProtoBuf types from `datetime.proto`.
## @author Tor Slettnes <tor@slett.net>
#===============================================================================

### Generated from `datetime.proto`
from cc.core.timeinterval import TimeInterval
from ..utils import proto_enum
from ..wellknown import DurationType
from .datetime_pb2 import TimeStruct, Weekday, Interval, TimeUnit


### Standard Python modules
import time

Weekday = proto_enum(Weekday)
TimeUnit = proto_enum(TimeUnit)

def encodeInterval(input: tuple[int, TimeUnit] | TimeUnit | Interval,
                   output: Interval|None = None) -> Interval:
    '''
    Convert a (COUNT, UNIT) tuple to an `Interval` message

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Interval()

    if isinstance(input, Interval):
        output.CopyFrom(input)

    elif input is None:
        output.count = 0
        output.unit = TimeUnit.ZERO_TIME

    elif isinstance(input, TimeUnit):
        output.count = 1
        output.unit = interval

    elif (isinstance(input, tuple) and
          len(input) == 2 and
          isinstance(input[1], TimeUnit)):

        output.count, output.unit = interval

    else:
        raise TypeError(
            "`interval` type must be one of: `Interval`, `TimeUnit`, or `tuple[int, TimeUnit]`")

    return output


def encodeTimeStruct(input: time.struct_time,
                     output: TimeStruct|None = None) -> TimeStruct:
    '''
    Convert a `time.time_struct` object to a `TimeStruct` message.

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = TimeStruct()

    output.year = input.tm_year
    output.month = input.tm_mon
    output.day = input.tm_mday
    output.hour = input.tm_hour
    output.minute = input.tm_min
    output.second = input.tm_sec
    output.weekday = (input.tm_wday+1)%7
    output.year_day = input.tm_yday
    output.is_dst = input.tm_isdst
    return output


def decodeTimeStruct (ts: TimeStruct) -> time.struct_time:
    '''
    Convert a `Timestruct` message to a `time.time_struct` object
    '''

    return time.time_struct((
        ts.year, ts.month, ts.day,
        ts.hour, ts.minute, ts.second,
        (ts.weekday-1)%7, ts.year_day, ts.is_dst))
