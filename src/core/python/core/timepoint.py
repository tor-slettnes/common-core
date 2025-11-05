'''
System time representation extending Python-native `float`.

Its purpose is to provide a dedicated, uniform, unambiguous timestamp
representation compatible with native Python types (e.g. `time`) - but also with
conversion methods to support additional common representations such as:

 - `datetime.datetime`
 - `time.struct_time`
 - ISO 8601 compliant date/time strings
 - `google.protobuf.Timestamp`
 - Scaled integer values (milliseconds/microseconds/nanoseconds)

## Usage Examples:

  ```
  >>> from cc.core.timepoint import TimePoint

  ### Create a new `TimePoint` instance from the current time.
  >>> tp = TimePoint.now()

  >>> type(tp)
  <class 'cc.core.timepoint.TimePoint'>

  >>> isinstance(tp, float)
  True

  ### This type has overriden `__str__()` and `__repr__()` methods
  >>> tp
  '2025-09-30 01:01:39.785'

  >>> print("%s" % tp)
  2025-09-30 01:01:39.785

  >>> print("%d" % tp)
  1759219299

  >>> print("%f" % tp)
  1759219299.785000

  ### ... as well as dedicated string conversions
  >>> tp.to_json_string()
  '2025-09-30T08:01:39.785Z'

  >>> tp.to_string(zone_suffix = ZoneSuffix.NAME, zone_delimiter = " ")
  '2025-09-30 01:01:39 PDT'

  ### Various import methods:
  >>> tp = TimePoint(time.time())
  >>> tp = TimePoint.from_milliseconds(time.time() * 1000)

  ### Also an `autoscaled_from()` method to automatically determine the scaling
  ### factor/resolution of an input
  >>> tp = TimePoint.autoscaled_from(epoch_time_with_unknown_scale)

  ### Even a `from_value()` method to create a TimePoint from a
  ### variant/uncontrolled input value, such as a JSON or YAML settings value.
  >>> tp = TimePoint.from_value("2025-10-01 00:00:00")
  >>> tp = TimePoint.from_value(1759302000000000, decimal_exponent=-6)

  ### Convert from `datetime` instances, assuming local time if naive
  >>> dt = datetime.datetime.now()
  >>> TimePoint.from_datetime(dt)
  '2025-09-30T08:49:36.666Z'

  ### Convert from `datetime` instances, with explicit zone information
  >>> dt = datetime.datetime.now(tz = datetime.timezone.utc)
  >>> TimePoint.from_datetime(dt)
  '2025-09-30T08:49:39.186Z'

  ### Convert from an ISO string, again assuming local time by default
  >>> TimePoint.from_string('2025-10-01 00:00:00')
  '2025-10-01T07:00:00.000Z'

  ### ...unless it ends in `Z`
  >>> TimePoint.from_string('2025-10-01 00:00:00Z')
  '2025-10-01T00:00:00.000Z'

  ### or the argument `assume_utc` is set
  >>> TimePoint.from_string('2025-10-01 00:00:00', assume_utc=True)
  '2025-10-01T00:00:00.000Z'

  ### Return time point as scaled integers
  >>> tp = TimePoint.from_string('2025-10-01 00:00:00')
  >>> tp.to_seconds()
  1759302000
  >>> tp.to_milliseconds()
  1759302000000
  >>> tp.to_microseconds()
  1759302000000000
  >>> tp.to_nanoseconds()
  1759302000000000000

  ### Convert from Epoch-based timestamp with automatic scaling
  >>> TimePoint.autoscaled_from(1759276800)
  '2025-09-30 17:00:00'
  >>> TimePoint.autoscaled_from(1759276800000)
  '2025-09-30 17:00:00'
  >>> TimePoint.autoscaled_from(1759276800000000)
  '2025-09-30 17:00:00'
  >>> TimePoint.autoscaled_from(1759276800000000000)
  '2025-09-30 17:00:00'

  ### Add/subtract time intervals (deltas)
  >>> tp = TimePoint.from_string('2025-10-01 00:00:00')
  >>> tp += 86400
  >>> tp
  '2025-10-02 00:00:00'

  >>> tp - '1y'
  '2024-10-01 00:00:00'

  ### Subtracting another TimePoint yields a TimeInterval
  >>> ti = tp - '2025-09-29 23:59:30'

  >>> ti
  '1d 30s'

  >>> type(ti)
  <class 'cc.core.timeinterval.TimeInterval'>

  >>> isinstance(ti, float):
  True
  ```
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

import enum
import time
import datetime
from .timeinterval import TimeInterval, TimeIntervalType, YEAR

TimePointType = time.struct_time|datetime.datetime

try:
    from google.protobuf.timestamp_pb2 import Timestamp as ProtoBufTimestamp
except ImportError:
    ProtoBufTimestamp = None
else:
    TimePointType |= ProtoBufTimestamp


class ZoneSuffix(enum.IntEnum):
    '''
    Time zone indicator.
    '''
    NONE   = 0  # Do not indicate timezone
    OFFSET = 1  # '+0100' for CET, '-0800' for PST
    NAME   = 2  # 'CET'/'CEST'/'PST'/'PDT'/etc, or 'UTC'
    ZULU   = 3  # 'Z' for UTC, otherwise nothing



class TimePoint (float):
    '''
    System time representation extending Python-native `float`.

    This is a drop-in replacement/wrapper for Python-native timestamps as
    returned by `time.time()` and friends, with conversion methods to/from
    various other time/date representations, including:

      - `datetime.datetime`
      - `time.struct_time`
      - ISO 8601 compliant date/time strings
      - `google.protobuf.Timestamp`
      - Scaled integer timestamps (milliseconds/microseconds/nanoseconds)

    Note that the first three of these types are intended for a slightly
    different use case: to represent calendar time, frequently in the local time
    zone, but also frequently with some ambiguity around this.  This class, in
    contrast, is a straightforward specialization of the native `float` type,
    and is as such intended primarily for representing linear/system time.
    '''

    def __str__(self):
        '''
        Return an informal string respresentation of this timestamp
        '''
        return self.to_string()


    def __repr__(self):
        '''
        Return a formal string represenation of this timestamp
        '''
        return f"'{self}'"


    def __add__ (self, input: TimeIntervalType):
        '''
        Add a time interval/delta value to this timestamp
        '''
        if isinstance(input, TimePointType):
            raise TypeError('Cannot add two time points. Did you intend to add a time interval instead?')

        return TimePoint(float(self) + TimeInterval.from_value(input))


    def __sub__(self, input: TimePointType|str|float|int):
        '''
        Subtract a time interval or other timepoint from this one.

        The input must be either one of:

         - a type supported by `TimeInterval.from_value()`, including a string
           generated from `TimeInterval().to_string()`, or

         - a type supported by `TimePoint.from_value()`, including an ISO 8601
           string such as that generated from `TimePoint().to_string()`.

        A plain number (`int`, `float`, or a numeric string), will be
        interpreted as a TimeInterval if its converted value is less than 10 years,
        or a TimePoint if 10 years or longer.

        If the input is interpreted as a `TimeInterval` the delta is returned as a
        new `TimePoint` value, and vice versa.
        '''

        if isinstance(input, str):
            try:
                ### Handle numeric strings as a number
                input = float(input)
            except ValueError:
                pass

        if isinstance(input, float|int):
            if TimeInterval(abs(input)) < 10*YEAR:
                input = TimeInterval(input)
            else:
                input = TimePoint.autoscaled_from(input)

        if isinstance(input, TimeIntervalType):
            return TimePoint(float(self) - TimeInterval.from_value(input))

        elif isinstance(input, TimePointType):
            return TimeInterval(float(self) - TimePoint.from_value(input))

        elif isinstance(input, str):
            try:
                ### Let's see if the input is a ISO 8601 time string
                return TimeInterval(float(self) - TimePoint.from_string(input))
            except ValueError:
                ### Nope. Perhaps a TimeInterval string, as generated from e.g.
                ### `TimeInterval.to_string()`?
                try:
                    return TimePoint(float(self) - TimeInterval.from_value(input))
                except ValueError:
                    raise ValueError(
                        "not a valid 8601 time string nor duration: " + input)

        else:
            return TypeError(
                "not a supported TimeInterval or TimePoint input type: "
                + type(input).__name__)

    @classmethod
    def now(cls) -> 'TimePoint':
        '''
        Return a new TimePoint object representing the current system time.
        '''
        return TimePoint(time.time())


    @classmethod
    def try_from(cls,
                 input: TimePointType|str|float|int,
                 fallback: object = None,
                 decimal_exponent: int|None = None,
                 assume_utc: bool = False,
                 ) -> 'TimePoint':
        '''
        Try creating a new TimePoint from a variant/undetermined type, such
        as an uncontrolled JSON or YAML settings value.

        If this fails, try again with the fallback value if provided. If the
        fallback is None or otherwise could not be converted to a TimePoint, it
        is returned unmodified.  In any case, no exception is raised.

        See `from_value()` for a description of the remaining input
        arguments and other details.
        '''

        try:
            return cls.from_value(input, decimal_exponent, assume_utc)
        except (ValueError, TypeError):
            pass

        if fallback is not None:
            try:
                return cls.from_value(fallback, decimal_exponent, assume_utc)
            except (ValueError, TypeError):
                pass

        return fallback


    @classmethod
    def from_value(cls,
                   input: TimePointType|str|float|int,
                   decimal_exponent: int|None = None,
                   assume_utc: bool = False,
                   ) -> 'TimePoint|None':

        '''
        Create a new TimePoint value from any supported time representation.

        * If the inputs is None, the current system time is returned.

        * If the input is an existing `TimePoint` value, it is returned intact.

        * Any other `int`, `float`, or string representation of a plain number
          is assumed to be an Epoch-based timestamp. Use the `decimal_exponent`
          input argument to specify explicit scaling if desired.

        * Any other string is passed to `from_datetime()`.  This may raise a
          ValueError if the string contents is not ISO 8601 compliant.

        * Any `datetime.datetime` input is passed on to `from_datetime()`.

        * Any `time.struct_time` value is passed on to `from_struct_time()`.

        * Any `google.protobuf.TimePoint` object is passed on to `from_protobuf()`.

        * Any other input type raises a TypeError.


        Input Arguments:

        @param input
            A supported timestamp representation

        @param decimal_exponent
            If provided, any numeric input is multiplied by ten to the specified
            power to yield seconds since Epoch. For instance, `0` means no
            scaling, `-3` means convert from milliseconcds. If not provided,
            numeric input values are passed on to `autoscaled_from()`.

        @param assume_utc
            Interpret "naive" time/date values as UTC and not local time
            representations.  This includes: an ISO 8601 string without a
            trailing `Z`, a `datetime.datetime` object without a `tzinfo`
            attribute, or a `time.struct_time` object.

        @returns
            A new `TimePoint` value.

        @exception ValueError
            The input is a string but could not be converted to `TimePoint`.

        @exception TypeError
            The input type is not supported for TimePoint conversion.
        '''

        if input is None:
            return TimePoint.now()

        elif isinstance(input, TimePoint):
            return input

        if isinstance(input, str):
            try:
                input = float(input)
            except ValueError:
                pass

        if isinstance(input, int|float):
            if isinstance(decimal_exponent, int):
                return cls(input * (10**decimal_exponent))
            else:
                return cls.autoscaled_from(input)

        elif isinstance(input, str):
            return cls.from_string(input, assume_utc)

        elif isinstance(input, datetime.datetime):
            return cls.from_datetime(input, assume_utc)

        elif isinstance(input, time.struct_time):
            return cls.from_struct_time(input, assume_utc)

        elif ProtoBufTimestamp and isinstance(input, ProtoBufTimestamp):
            return cls.from_protobuf(input)

        else:
            raise TypeError("Could not convert %s object to TimePoint"%
                            (type(input).__name__,))



    AUTOSCALE_SECONDS_UPPER_LIMIT = 31536000000

    @classmethod
    def autoscaled_from(cls, input: int|float) -> 'TimePoint':
        '''
        Create a new TimePoint from an Epoch-based input with automatic
        scaling/resolution.

        The scaling factor is determined as follows:

         - If the input is 0 or negative, UNIX Epoch is assumed

         - If the scalar is positive, it is first converted to seconds by
           repeatedly dividing by 1000, until it falls below
           `AUTOSCALE_SECONDS_UPPER_LIMIT`.

         - The resulting timestamp is bounded between January 1st 1971
           and May 3rd 2969.
        '''

        if isinstance(input, int|float):
            while (input >= cls.AUTOSCALE_SECONDS_UPPER_LIMIT):
                input /= 1000.0

        return TimePoint(max(0, input))



    @classmethod
    def from_seconds(cls, seconds: int|float) -> 'TimePoint':
        '''
        Return a new TimePoint given number of seconds since UNIX Epoch.

        This is effectively the same as `TimePoint(seconds)`, provided for completeness.
        '''
        return TimePoint(seconds)


    @classmethod
    def from_milliseconds(cls, milliseconds: int|float) -> 'TimePoint':
        '''
        Return a new TimePoint given number of milliseconds since UNIX Epoch.
        '''
        return TimePoint(milliseconds / 1e3)


    @classmethod
    def from_microseconds(cls, microseconds: int|float) -> 'TimePoint':
        '''
        Return a new TimePoint given number of microseconds since UNIX Epoch.
        '''
        return TimePoint(microseconds / 1e6)


    @classmethod
    def from_nanoseconds(cls, nanoseconds: int|float) -> 'TimePoint':
        '''
        Return a new TimePoint given number of nanoseconds since UNIX Epoch.
        '''
        return TimePoint(nanoseconds / 1e9)


    @classmethod
    def from_epoch(cls, *,
                   seconds      : int|float|None = None,
                   milliseconds : int|float|None = None,
                   microseconds : int|float|None = None,
                   nanoseconds  : int|float|None = None) -> 'TimePoint':
        '''
        Return a new TimePoint from a combination of Epoch-based inputs.

        Each input is scaled to the corresponding fraction of a second,
        then added together to form a new TimePoint instance.

        @param seconds
            Added to the result.

        @param milliseconds
            Divided by 1000, then added to the result.

        @param microseconds
            Divided by 1e6, then added to the result.

        @param nanoseconds
            Divided by 1e9, then added to the result.

        @return
            A new TimePoint instance
        '''

        return TimePoint(
            seconds
            + (milliseconds or 0) / 1e3
            + (microseconds or 0) / 1e6
            + (nanoseconds or 0) / 1e9)


    @classmethod
    def from_struct_time(cls,
                         input: time.struct_time,
                         is_utc: bool = False):
        '''
        Create a new TimePoint object from a `time.struct_time` value.

        @param input
            An existing `time.struct_time` instance, as returned by
            `time.localtime()` or `time.gmtime()`.

        @param is_utc
            Interpret the input as UTC, not local time.
        '''

        timestamp = time.mktime(input)

        # `time.mktime()` assumes the input is local time, so if we instead
        # provided UTC we now need to subtract the delta.
        if is_utc:
            timestamp -= cls.tz_offset(input)

        return TimePoint(timestamp)


    @classmethod
    def from_datetime(cls,
                      input: datetime.datetime,
                      assume_utc: bool = False,
                      ) -> 'TimePoint':
        '''
        Create a new TimePoint object from a `datetime.datetime` value.

        @param input
            An existing `datetime.datetime` value.

        @param assume_utc
            If `input` is a naive `datetime.datetime` value without a `tzinfo`
            attribute, assume it represents UTC rather than defaulting to local
            time.
        '''

        if not input.tzinfo and assume_utc:
            input = input.replace(tzinfo = datetime.timezone.utc)

        return TimePoint(input.timestamp())


    @classmethod
    def from_string(cls,
                    input: str,
                    assume_utc: bool = False,
                    ) -> 'TimePoint':
        '''
        Create a new TimePoint from a ISO 8601 formatted date/time string.
        See `datetime.datetime.fromisoformat()` for details on expected format.

        @param input
            Date/time input as a ISO 8601 formatted string.  If the string ends
            in `Z` it is interpreted as UTC time, otherwise it is interpreted
            according to the `assume_utc` argument.

        @param assume_utc
            Assume the input string represents UTC rather than local time, even
            if it does not end in `Z`.
        '''

        return TimePoint.from_datetime(
            input = datetime.datetime.fromisoformat(input),
            assume_utc = assume_utc)



    @classmethod
    def from_protobuf(cls,
                      input: ProtoBufTimestamp):
        '''
        Create a new TimePoint from a `google.protobuf.TimePoint` instance.

        @param input
            An existing instance of `google.protobuf.TimePoint`
        '''

        return TimePoint.from_epoch(
            seconds = input.seconds,
            nanoseconds  = input.nanos)


    @classmethod
    def tz_offset(cls,
                  timestruct: time.struct_time,
                  is_utc: bool = False):
        '''

        Return the delta between the specified `timestruct` and the corresponding UTC time
        '''

        if is_utc:
            return 0

        elif timestruct.tm_isdst:
            return time.altzone

        else:
            return time.timezone

    def to_seconds(self) -> int:
        '''
        Return this timestamp as an integer denoting number of seconds since
        UNIX Epoch
        '''
        return int(self)


    def to_milliseconds(self) -> int:
        '''
        Return this timestamp as an integer denoting number of milliseconds
        since UNIX Epoch
        '''
        return int(self * 1000)


    def to_microseconds(self) -> int:
        '''
        Return this timestamp as an integer denoting number of microseconds
        since UNIX Epoch
        '''
        return int(self * 1e6)


    def to_nanoseconds(self) -> int:
        '''
        Return this timestamp as an integer denoting number of nanoseconds
        since UNIX Epoch
        '''
        return int(self * 1e9)


    def to_format(self,
                  format: str,
                  local : bool = True) -> str:
        '''
        Return a custom string representation of this timestamp, using `strftime()`.

        @param format
            Format string, passed to `time.strftime()`

        @param local
            Return local time (default).  If `False`, return UTC instead.
        '''

        return time.strftime(
            format,
            time.localtime(self) if local else time.gmtime(self))


    def to_json_string(self, *,
                      decimals = 3) -> str:
        '''
        Return this timestamp as an ISO 8601 string representing UTC,
        suitable for unambiguous represention in containers such as JSON.

        @param decimals
            Sub-second precision, e.g., 3 for millseconds, 9 for nanoseconds.

        @returns
             UTC calendar time representation of this timestamp, formatted as
             `{year}-{month}-{day}T{hours}:{minutes}:{seconds}Z`, where
             `{seconds}` may contain a `.` for the fractional component.

        For additional output format controls, see `to_string()`.
        '''

        return self.to_string(utc = True,
                              time_delimiter = "T",
                              zone_delimiter = "",
                              zone_suffix = ZoneSuffix.ZULU,
                              decimals = decimals)


    def to_string(self, *,
                  utc           : bool = False,
                  time_delimiter: str  = " ",
                  decimals      : int  = 0,
                  zone_suffix   : ZoneSuffix = ZoneSuffix.ZULU,
                  zone_delimiter: str = "") -> str:
        '''
        Return an ISO 8601 string representation of this timestamp.

        @param time_delimiter
            Delimiter between date and time. ISO 8601 allows space (' ') or 'T'.

        @param decimals
            Sub-second precision, e.g., 3 for millseconds, 9 for nanoseconds.

        @param utc
            Convert to UTC rather than local time.

        @param zone_suffix
            Append timezone suffix. One of:
            - ZoneSuffix.NONE   - No suffix
            - ZoneSuffix.OFFSET - ISO zone indicator, e.g. '+0100' for CET, '-0800' for PST
            - ZoneSuffix.NAME   - Abbreviated effective none name, e.g. `PST` or `PDT`.
                                  (Not ISO 8601 compliant, nor globally unique).
            - ZoneSuffix.ZULU   - `Z` if `utc` is set, or nothing otherwise.

        @param zone_delimiter
            Delimiter between time and zone name.
        '''

        timestruct = self.to_struct_time(utc = utc)
        parts = [ time.strftime(f"%F{time_delimiter}%T", timestruct) ]

        if decimals:
            fraction = float(self) - int(self)
            parts.append(".%0*d"%(decimals, (10**decimals) * fraction))

        match zone_suffix:
            case ZoneSuffix.OFFSET:
                parts.append(zone_delimiter)
                offset = self.tz_offset(timestruct, utc)
                parts.append("%+03d%02d"%(
                    -offset / 3600,
                    (-offset % 3600) / 60))

            case ZoneSuffix.NAME if utc:
                parts.append(zone_delimiter)
                parts.append("UTC")

            case ZoneSuffix.NAME:
                parts.append(zone_delimiter)
                name = time.tzname[timestruct.tm_isdst]

                ### On Windows we get the full name, e.g. "Pacific Daylight Time"
                ### or "Central European Summer Time".   Extract only caps.
                parts.extend([c for c in name if c.isupper()])

            case ZoneSuffix.ZULU if utc:
                parts.append(zone_delimiter)
                parts.append("Z")

        return "".join(parts)

    def to_struct_time(self, *, utc = False) -> time.struct_time:
        '''
        Convert this timestamp to a `time.struct_time` instance.

        @param utc
            Convert to UTC rather than local time.

        @return
            New `time.struct_time` instance.
        '''
        return time.gmtime(self) if utc else time.localtime(self)


    def to_datetime(self, *, utc = False) -> datetime.datetime:
        '''
        Convert this timestamp to a `datetime.datetime` object.

        @param utc
            Convert to UTC rather than local time, and populate the result with
            a corresponding `tzinfo` attribute.

        @return
            A new `datetime.datetime` instance. This will be naive unless `utc`
            is set.
        '''

        return datetime.datetime.fromtimestamp(
            self,
            datetime.timezone.utc if utc else None)


    def to_protobuf(self) -> ProtoBufTimestamp:
        '''
        Convert this timestamp to the well-known ProtoBuf type
        `google.protobuf.Timestamp`.

        @return
             New `google.protobuf.Timestamp` instance.
        '''
        from google.protobuf.timestamp_pb2 import Timestamp
        return Timestamp(
            seconds = int(self),
            nanos = int((self - int(self)) * 1e9))


TimePointType |= TimePoint

