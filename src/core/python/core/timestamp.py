'''
Timestamp representation extending Python-native `float`.

Its purpose is to provide a dedicated, uniform, unambiguous timestamp
representation compatible with native Python types (e.g. `time`) - but also with
conversion methods to support additional common representations such as:

 - `datetime.datetime`
 - `time.struct_time`
 - ISO 8601 compliant date/time strings
 - `google.protobuf.Timestamp`
 - Scaled integer timestamps (milliseconds/microseconds/nanoseconds)

## Usage Examples:

  ```
  >>> from cc.core.timestamp import Timestamp

  ### Create a new `Timestamp` instance from the current time.
  >>> ts = Timestamp.now()
  >>> isinstance(ts, float)
  True

  ### This type has overriden `__str__()` and `__repr__()` methods
  >>> ts
  '2025-09-30T08:01:39.785Z'

  >>> print("%s" % ts)
  2025-09-30T08:01:39.785Z

  >>> print("%d" % ts)
  1759219299

  >>> print("%f" % ts)
  1759219299.785000


  ### Also the type persists when adding/subtracting time deltas
  >>> ts += 86400
  >>> ts
  '2025-10-01T08:01:39.785Z'

  ### Various import methods:
  >>> ts = Timestamp(time.time())
  >>> ts = Timestamp.from_milliseconds(time.time() * 1000)

  ### Also an `autoscaled_from()` method to automatically determine the scaling
  ### factor/resolution of an input
  >>> ts = Timestamp.autoscaled_from(epoch_time_with_unknown_scale)

  ### Even a `from_value()` method to create a Timestamp from a
  ### variant/uncontrolled input value, such as a JSON or YAML settings value.
  >>> ts = Timestamp.from_value("2025-10-01 00:00:00")
  >>> ts = Timestamp.from_value(1759302000000000, decimal_exponent=-6)

  ### Convert from `datetime` instances, assuming local time if naive
  >>> dt = datetime.datetime.now()
  >>> Timestamp.from_datetime(dt)
  '2025-09-30T08:49:36.666Z'

  ### Convert from `datetime` instances, with explicit zone information
  >>> dt = datetime.datetime.now(tz = datetime.timezone.utc)
  >>> Timestamp.from_datetime(dt)
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

  ### Return timestamp as scaled integers
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
  >>> Timestamp.autoscaled_from(1759276800)
  '2025-10-01T00:00:00.000Z'
  >>> Timestamp.autoscaled_from(1759276800000)
  '2025-10-01T00:00:00.000Z'
  >>> Timestamp.autoscaled_from(1759276800000000)
  '2025-10-01T00:00:00.000Z'
  >>> Timestamp.autoscaled_from(1759276800000000000)
  '2025-10-01T00:00:00.000Z'

  ```
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

import enum
import time
import datetime
from .duration import Duration, DurationType

TimestampType = float|int
DateTimeType  = str|time.struct_time|datetime.datetime

try:
    from google.protobuf.timestamp_pb2 import Timestamp as ProtoBufTimestamp
except ImportError:
    ProtoBufTimestamp = None
else:
    TimestampType |= ProtoBufTimestamp


class ZoneSuffix(enum.IntEnum):
    '''
    Time zone indicator.
    '''
    NONE   = 0  # Do not indicate timezone
    OFFSET = 1  # '+0100' for CET, '-0800' for PST
    NAME   = 2  # 'CET'/'CEST'/'PST'/'PDT'/etc, or 'UTC'
    ZULU   = 3  # 'Z' for UTC, otherwise nothing



class Timestamp (float):
    '''
    Timestamp representation extending Python-native `float`.

    This is a drop-in replacement/wrapper for Python-native timestamps as
    returned by `time.time()` and friends, with conversion methods to/from
    various other time/date representations, including:

      - `datetime.datetime`
      - `time.struct_time`
      - ISO 8601 compliant date/time strings
      - `google.protobuf.Timestamp`
      - Scaled integer timestamps (milliseconds/microseconds/nanoseconds)

    Note that the first three of these types are indented for a slightly
    different use case: to represent calendar time, frequently in the local time
    zone, but also frequently with some ambiguity around this.  This class, in
    contrast, is a straightforward specialization of the native `float` type,
    and is as such intended primarily for representing linear/system time.
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
        return Timestamp(float(self) + Duration.from_value(duration))


    def __sub__(self, duration: DurationType):
        '''
        Subtract a duration (relative offset) from this timestamp
        '''
        return Timestamp(float(self) - Duration.from_value(duration))


    @classmethod
    def now(cls) -> 'Timestamp':
        '''
        Return a new Timestamp object representing the current system time.
        '''
        return Timestamp(time.time())


    @classmethod
    def try_from(cls,
                 input: TimestampType|DateTimeType,
                 fallback: object = None,
                 decimal_exponent: int|None = None,
                 assume_utc: bool = False,
                 ) -> 'Timestamp':
        '''
        Try creating a new Timestamp from a variant/undetermined type, such
        as an uncontrolled JSON or YAML settings value.

        If this fails, try again with the fallback value if provided. If the
        fallback is None or otherwise could not be converted to a Timestamp, it
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
                   input: TimestampType|DateTimeType,
                   decimal_exponent: int|None = None,
                   assume_utc: bool = False,
                   ) -> 'Timestamp|None':

        '''
        Create a new Timestamp value from any supported time representation.

        * If the input is an existing `Timestamp` value, it is returned intact.

        * Any other `int`, `float`, or string representation of a plain number
          is assumed to be an Epoch-based timestamp. Use the `decimal_exponent`
          input argument to specify explicit scaling if desired.

        * Any other string is passed to `from_datetime()`.  This may raise a
          ValueError if the string contents is not ISO 8601 compliant.

        * Any `datetime.datetime` input is passed on to `from_datetime()`.

        * Any `time.struct_time` value is passed on to `from_struct_time()`.

        * Any `google.protobuf.Timestamp` object is passed on to `from_protobuf()`.

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
            A new `Timestamp` value.

        @exception ValueError
            The input is a string but could not be converted to `Timestamp`.

        @exception TypeError
            The input type is not supported for Timestamp conversion.
        '''

        if isinstance(input, Timestamp):
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
            raise TypeError("Could not convert %s object to Timestamp"%
                            (type(input).__name__,))



    AUTOSCALE_SECONDS_UPPER_LIMIT = 31536000000

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

         - The resulting timestamp is bounded between January 1st 1971
           and May 3rd 2969.
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
