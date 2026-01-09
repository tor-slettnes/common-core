#!/bin/echo Do not invoke directly.
#===============================================================================
## @file datetime.py
## @brief Support for ProtoBuf types from `datetime.proto`.
## @author Tor Slettnes
#===============================================================================

### Standard Python modules
import time
import datetime

### Common Core modules
from cc.core.timeutils import TimeInterval
from ..utils import native_enum_from_proto
from ..wellknown import DurationType

### Generated from `.../protos/cc/protobuf/datetime/datetime.proto`
from .datetime_pb2 import TimeStruct, Weekday, Interval, TimeUnit

Weekday = native_enum_from_proto(Weekday)
TimeUnit = native_enum_from_proto(TimeUnit)

TimeDeltaType = tuple[int, TimeUnit] | TimeUnit | Interval | datetime.timedelta


def encodeInterval(input: TimeDeltaType,
                   output: Interval|None = None) -> Interval:
    '''
    Convert a (COUNT, UNIT) tuple to an `Interval` message

    If `output` is provided it is populated in place, otherwise a new instance
    is created. In either case, the resulting value is returned.
    '''

    if output is None:
        output = Interval()

    if input is None:
        output.count = 0
        output.unit = TimeUnit.ZERO_TIME

    elif isinstance(input, Interval):
        output.CopyFrom(input)

    elif isinstance(input, TimeUnit):
        output.count = 1
        output.unit = interval

    elif (isinstance(input, tuple) and
          len(input) == 2 and
          isinstance(input[1], TimeUnit)):

        output.count, output.unit = interval

    elif isinstance(input, datetime.timedelta):
        if input == datetime.timedelta.max:
            output.unit = TimeUnit.ETERNITY

        elif microseconds := input.microseconds:
            if microseconds % 1000:
                output.unit = TimeUnit.MICROSECONDS
                output.count = microseconds + (input.total_seconds() * 1000000)

            else:
                output.unit = TimeUnit.MILLISECONDS
                output.count = (microseconds // 1000) + (input.total_seconds() * 1000)

        elif input.seconds:
            seconds = input.total_seconds()

            if seconds % 60:
                output.unit = TimeUnit.SECOND
                output.count = seconds

            elif seconds % 3600:
                output.unit = TimeUnit.MINUTE
                output.count = seconds // 60

            else:
                output.unit = TimeUnit.HOUR
                output.count = seconds // 3600

        elif input.days:
            output.unit = TimeUnit.DAY
            output.count = input.days

        else:
            output.unit = TimeUnit.ZERO_TIME

    else:
        raise TypeError(
            "`interval` type must be one of: `Interval`, `TimeUnit`, or `tuple[int, TimeUnit]`")

    return output


def decodeInterval(input: Interval) -> datetime.timedelta:
    '''
    Convert a ProtoBuf `Interval` message to a `datetime.timedelta` value.
    '''

    match(input.unit):
        case TimeUnit.ZERO_TIME:
            return datetime.timedelta()

        case TimeUnit.NANOSECOND:
            return datetime.timedelta(microseconds = input.count / 1000)

        case TimeUnit.MICROSECOND:
            return datetime.timedelta(microseconds = input.count)

        case TimeUnit.MILLISECOND:
            return datetime.timedelta(milliseconds = input.count)

        case TimeUnit.SECOND:
            return datetime.timedelta(seconds = input.count)

        case TimeUnit.MINUTE:
            return datetime.timedelta(minutes = input.count)

        case TimeUnit.HOUR:
            return datetime.timedelta(hours = input.count)

        case TimeUnit.DAY:
            return datetime.timedelta(days = input.count)

        case TimeUnit.WEEK:
            return datetime.timedelta(weeks = input.count)

        case TimeUnit.MONTH:
            # Approximate
            return datetime.timedelta(days = 30 * input.count)

        case TimeUnit.YEAR:
            # Approximate
            return datetime.timedelta(days = 365.2425 * input.count)

        case TimeUnit.ETERNITY:
            return datetime.timedelta.max

        case _:
            raise ValueError(
                "Invalid time unit enumeration %d in `%s` object: %s"
                %(input.unit, input.DESCRIPTOR.full_name, input))


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
