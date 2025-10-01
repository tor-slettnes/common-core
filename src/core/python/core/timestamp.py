'''
Timestamp representation extending Python-native `float`.

Its primary purpose is to provide a dedicated, uniform, unambiguous timestamp
representation compatible with native Python types (e.g. `time`) - but also with
conversion methods to support additional common representations such as:

 - `datetime.datetime`
 - `time.struct_time`
 - ISO 8601 compliant date/time strings
 - `google.protobuf.Timestamp`
 - Scaled integer timestamps (milliseconds/microseconds/nanoseconds)

Note that the first three of these types are indented for a slightly different
use case: to represent calendar time, frequently in the local time zone, but
also frequently with some ambiguity around this. (For example, these may all be
"naive" with regards to timezone information).

This class, in contrast, is a straightforward specialization of the native
`float` type, and is as such primarily intended for representing linear/system
time.

  ```
  >>> from cc.core.timestamp import Timestamp

  ### Create a new `Timestamp` instance from the current time.
  >>> ts = Timestamp.now()
  >>> isinstance(ts, float)
  True

  ### This type has overriden `__str__()` and `__repr__()` methods
  >>> ts
  '2025-09-30T08:01:39.785Z'

  ### Also the type persists when adding/subtracting time deltas
  >>> ts += 86400
  >>> ts
  '2025-10-01T08:01:39.785Z'

  ### Various import methods:
  >>> ts1 = Timestamp(time.time())
  >>> ts2 = Timestamp.from_milliseconds(time.time() * 1000)

  ### Also an `autoscaled_from()` method to automatically determine the scaling
  ### factor/resolution of an input
  >>> ts3 = Timestamp.autoscaled_from(some_value_maybe_seconds_maybe_millis_maybe_nanos)

  ### Even a `from_value()` method to create a Timestamp from a
  ### variant/uncontrolled input value, such as a JSON or YAML settings value.
  >>> ts4 = Timestamp.from_value("2025-10-01 00:00:00")
  >>> ts5 = Timestamp.from_value(1759302000000000, decimal_exponent=-6)

  ### Convert from `datetime` instances, assuming local time if naive
  >>> dt6 = datetime.datetime.now()

  ### Convert from `datetime` instances, with explicit zone information
  >>> dt7 = datetime.datetime.now(tz = datetime.timezone.utc)

  >>> Timestamp.from_datetime(dt6)
  '2025-09-30T08:49:36.666Z'

  >>> Timestamp.from_datetime(dt7)
  '2025-09-30T08:49:39.186Z'

  ### Convert from an ISO string, again assuming local time by default
  >>> Timestamp.from_string('2025-10-01 00:00:00')
  '2025-10-01T07:00:00.000Z'

  ### ...unless it ends in `Z`
  >>> Timestamp.from_string('2025-10-01 00:00:00Z')
  '2025-10-01T00:00:00.000Z'

  ### or the argument `assume_utc` is set
  >>> Timestamp.from_string('2025-10-01 00:00:00', assume_utc=True)
  '2025-10-01T00:00:00.000Z'

  ### Return timestamp as milliseconds, microseconds, nanoseconds
  >>> ts = Timestamp.from_string('2025-10-01 00:00:00')
  >>> ts.to_seconds()
  1759302000
  >>> ts.to_milliseconds()
  1759302000000
  >>> ts.to_microseconds()
  1759302000000000
  >>> ts.to_nanoseconds()
  1759302000000000000

  ### Convert from Epoch-based timestamp with automatic scaling
  >>> import time
  >>> t = time.time()
  >>> Timestamp.autoscaled_from(t)
  '2025-09-30T08:58:26.761Z'
  >>> Timestamp.autoscaled_from(t * 1000)
  '2025-09-30T08:58:26.761Z'
  >>> Timestamp.autoscaled_from(t * 1000000)
  '2025-09-30T08:58:26.761Z'
  >>> Timestamp.autoscaled_from(t * 1000000000)
  '2025-09-30T08:58:26.761Z'

  ```
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

from enum import IntEnum
import time
import datetime
import re

TimestampType = float|int
DateTimeType  = str|time.struct_time|datetime.datetime
DurationType  = float|int

try:
    from google.protobuf.timestamp_pb2 import Timestamp as ProtoBufTimestamp
    from google.protobuf.duration_pb2 import Duration as ProtoBufDuration
except ImportError:
    ProtoBufTimestamp = None
    ProtoBufDuration = None
else:
    TimestampType |= ProtoBufTimestamp
    DurationType |= ProtoBufDuration


class ZoneSuffix(IntEnum):
    '''
    Time zone indicator.
    '''
    NONE   = 0  # Do not indicate timezone
    OFFSET = 1  # '+0100' for CET, '-0800' for PST
    NAME   = 2  # 'CET'/'CEST'/'PST'/'PDT'/etc, or 'UTC'
    ZULU   = 3  # 'Z' for UTC, otherwise nothing


class Missing:
    '''
    Dummy type to indicate missing argument value.
    '''
    pass


class Timestamp (float):
    '''
    Timestamp representation extending Python-native `float`.

    This is a drop-in replacement/wrapper for Python-native timestamps as
    returned by `time.time()` and friends.  Its primary purpose is to provide
    dedicated, uniform, unambiguous time representation, compatible with native
    Python modules (e.g. `time`).  At the same time this class provides accessor
    methods to convert to/from various other time/date representations, including:

      - `datetime.datetime`
      - `time.struct_time`
      - ISO 8601 compliant date/time strings
      - `google.protobuf.Timestamp`
      - Scaled integer timestamps (milliseconds/microseconds/nanoseconds)

    Note that the first three of these types are indented for a slightly
    different use case: to represent calendar time, frequently in the local time
    zone, but also frequently with some ambiguity around this.  This class, in
    contrast, is primarily intended for linear/system time.
    '''

    # def __new__(cls, input: TimestampType):
    #     if isinstance(input, int|float):
    #         return super().__new__(cls, input)

    #     elif ProtoBufTimestamp and isinstance(input, ProtoBufTimestamp):
    #         return type(self).from_protobuf(input)

    #     else:
    #         raise TypeError("Cannot convert %r object to Timestamp"%(type(input).__name__,))

    def __str__(self):
        '''
        Return an informal string respresentation of this timestamp
        '''
        return self.to_utc_string()

    def __repr__(self):
        '''
        Return a formal string represenation of this timestamp
        '''
        return f"'{self}'"

    def __add__ (self, duration: DurationType):
        '''
        Add a duration (relative offset) to this timestamp
        '''

        if isinstance(duration, (int, float)):
            return Timestamp(float(self) + duration)

        elif ProtoBufDuration and isinstance(duration, ProtoBufDuration):
            return Timestamp(float(self) + (duration.ToNanoseconds() / 1e9))

        else:
            raise TypeError("Cannot add %s object to Timestamp"%(type(duration).__name__,))

    def __sub__(self, duration: DurationType):
        '''
        Subtract a duration (relative offset) from this timestamp
        '''

        if isinstance(duration, (int, float)):
            return Timestamp(float(self) - duration)

        elif ProtoBufDuration and isinstance(duration, ProtoBufDuration):
            return Timestamp(float(self) - (duration.ToNanoseconds() / 1e9))

        else:
            raise TypeError("Cannot subtract %s object from Timestamp"%(type(duration).__name__,))


    @classmethod
    def now(cls) -> 'Timestamp':
        '''
        Return a new Timestamp object representing the current system time.
        '''
        return Timestamp(time.time())


    @classmethod
    def from_value(cls,
                   input: str|int|float,
                   fallback: object|None|Missing = Missing,
                   assume_utc: bool = False,
                   decimal_exponent: int|None = None,
                   ) -> 'Timestamp':
        '''
        Create a new Timestamp from a variant/undetermined type, such as an
        uncontrolled JSON or YAML settings value.

        * If the input is an `int`, `float`, or a string representation of a
          plain number, it is assumed to be an Epoch-based timestamp. In this
          case:

           - If `decimal_exponent` is specified, the number is scaled to seconds
             by multiplying by ten to the specified power.  For instance, if
             `decimal_exponent` is `-3` the number is assumed to represent
             milliseconds since Epoch, and multiplied by 0.001 to yield seconds.

           - Otherwise, the number is passed on to `autoscaled_from()` to
             dynamically determine the resolution.

        * If the input is a ISO 8601 formatted string, with an arbitrary
          date/time separator and with or without fractional seconds or a
          trailing `Z`, it is passed to `from_string()`.

        * Otherwise,  if a `fallback` value is provided:

           - If `fallback` is a number or a string, it is processed
             similar to `input`.

           - Otherwise, `fallback` is returned unmodified

        * If all of the above fails, a `TypeError` or `ValueError` is raised,
          depending on the type of `input`.
        '''

        if isinstance(input, str):
            try:
                input = float(input)
            except ValueError:
                pass

        if isinstance(input, int|float):
            if isinstance(decimal_exponent, int):
                return input * (10**decimal_exponent)
            else:
                return cls.autoscaled_from(input)

        if isinstance(input, str):
            try:
                return cls.from_string(input, assume_utc)
            except ValueError:
                pass

        if fallback is not Missing:
            if isinstance(fallback, Timestamp):
                return fallback

            elif isinstance(fallback, str|int|float):
                return Timestamp.from_value(
                    fallback,
                    assume_utc = assume_utc,
                    decimal_exponent = decimal_exponent)

            else:
                return fallback

        elif isinstance(input, str):
            raise ValueError("Could not convert string to Timestamp: %r"%
                             (input,))

        else:
            raise TypeError("Could not convert %s object to Timestamp"%
                            (type(input).__name__,))


    AUTOSCALE_SECONDS_UPPER_LIMIT = 1e11

    @classmethod
    def autoscaled_from(cls, input: int|float) -> 'Timestamp':
        '''
        Create a new Timestamp from an Epoch-based input with automatic
        scaling/resolution.

        The scaling factor is determined as follows:

         - If the input is 0 or negative, UNIX Epoch is assumed

         - If the scalar is positive, it is first converted to seconds by
           repeatedly dividing by 1000, until it falls below
           `AUTOSCALE_SECONDS_UPPER_LIMIT`.

         - The resulting timestamp is bounded between March 3, 1973 and
           November 16, 5138.
        '''

        if isinstance(input, int|float):
            while (input >= cls.AUTOSCALE_SECONDS_UPPER_LIMIT):
                input /= 1000.0

        return Timestamp(max(0, input))



    @classmethod
    def from_seconds(cls, seconds: int|float) -> 'Timestamp':
        '''
        Return a new Timestamp given number of seconds since UNIX Epoch.

        This is effectively the same as `Timestamp(seconds)`, provided for completeness.
        '''
        return Timestamp(seconds)


    @classmethod
    def from_milliseconds(cls, milliseconds: int|float) -> 'Timestamp':
        '''
        Return a new Timestamp given number of milliseconds since UNIX Epoch.
        '''
        return Timestamp(milliseconds / 1e3)


    @classmethod
    def from_microseconds(cls, microseconds: int|float) -> 'Timestamp':
        '''
        Return a new Timestamp given number of microseconds since UNIX Epoch.
        '''
        return Timestamp(microseconds / 1e6)


    @classmethod
    def from_nanoseconds(cls, nanoseconds: int|float) -> 'Timestamp':
        '''
        Return a new Timestamp given number of nanoseconds since UNIX Epoch.
        '''
        return Timestamp(nanoseconds / 1e9)


    @classmethod
    def from_epoch(cls, *,
                   seconds      : int|float|None = None,
                   milliseconds : int|float|None = None,
                   microseconds : int|float|None = None,
                   nanoseconds  : int|float|None = None) -> 'Timestamp':
        '''
        Return a new Timestamp from a combination of Epoch-based inputs.

        Each input is scaled to the corresponding fraction of a second,
        then added together to form a new Timestamp instance.

        @param seconds
            Added to the result.

        @param milliseconds
            Divided by 1000, then added to the result.

        @param microseconds
            Divided by 1e6, then added to the result.

        @param nanoseconds
            Divided by 1e9, then added to the result.

        @return
            A new Timestamp instance
        '''

        return Timestamp(
            seconds
            + (milliseconds or 0) / 1e3
            + (microseconds or 0) / 1e6
            + (nanoseconds or 0) / 1e9)


    @classmethod
    def from_struct_time(cls,
                         input: time.struct_time,
                         is_utc: bool = False):
        '''
        Create a new Timestamp object from a `time.struct_time` value.

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

        return Timestamp(timestamp)


    @classmethod
    def from_datetime(cls,
                      input: datetime.datetime,
                      assume_utc: bool = False,
                      ) -> 'Timestamp':
        '''
        Create a new Timestamp object from a `datetime.datetime` value.

        @param input
            An existing `datetime.datetime` value.

        @param assume_utc
            Even if `input` is a naive `datetime.datetime` value without a
            `tzinfo` attribute, assume it represents UTC rather than defaulting
            to local time.
        '''

        if not input.tzinfo and assume_utc:
            input = input.replace(tzinfo = datetime.timezone.utc)

        return Timestamp(input.timestamp())


    @classmethod
    def from_string(cls,
                    input: str,
                    assume_utc: bool = False,
                    ) -> 'Timestamp':
        '''
        Create a new Timestamp from a ISO 8601 formatted date/time string.
        See `datetime.datetime.fromisoformat()` for details on expected format.

        @param input
            Date/time input as a ISO 8601 formatted string.  If the string ends
            in `Z` it is interpreted as UTC time, otherwise it is interpreted
            according to the `assume_utc` argument.

        @param assume_utc
            Assume the input string represents UTC rather than local time, even
            if it does not end in `Z`.
        '''

        return Timestamp.from_datetime(
            input = datetime.datetime.fromisoformat(input),
            assume_utc = assume_utc)



    @classmethod
    def from_protobuf(cls,
                      input: ProtoBufTimestamp):
        '''
        Create a new Timestamp from a `google.protobuf.Timestamp` instance.

        @param input
            An existing instance of `google.protobuf.Timestamp`
        '''

        return Timestamp(input.seconds + (input.nanos * 1e-9))


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


    def to_utc_string(self, *,
                      decimals = 3) -> str:
        '''
        Return this timestamp as an ISO 8601 formatted string representing UTC.

        @param decimals
            Sub-second precision, e.g., 3 for millseconds, 9 for nanoseconds.

        @returns
             UTC calendar time representation of this timestamp, formatted as
             `{year}-{month}-{day}T{hours}:{minutes}:{seconds}Z`, where
             `{seconds}` may contain a `.` for the fractional component.

        For additional output format controls, see `to_string()`.
        '''

        return self.to_string(utc = True, decimals = decimals)


    def to_local_string(self, *,
                        decimals = 3) -> str:
        '''
        Return this timestamp as an ISO 8601 formatted string representing local time.

        @param decimals
            Sub-second precision, e.g., 3 for millseconds, 9 for nanoseconds.

        @returns
             Local calendar time representation of this timestamp, formatted as
             `{year}-{month}-{day}T{hours}:{minutes}:{seconds}`, where
             `{seconds}` may contain a `.` for the fractional component.

        For additional output format controls, see `to_string()`.
        '''

        return self.to_string(utc = False, decimals = decimals)


    def to_string(self, *,
                  utc           : bool,
                  time_delimiter: str  = "T",
                  decimals      : int  = 3,
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
                abbrev = filter(lambda c: c.isupper(), name)

                parts.append(abbrev)

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
        from google.protobuf.timestamp_pb2 import Timestamp as ProtoBufTime
        return ProtoBufTime(seconds = int(self),
                            nanos = int((self - int(self)) * 1e9))
