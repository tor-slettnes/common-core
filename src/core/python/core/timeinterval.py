'''
Relative time interval representation extending Python-native `float`.

Its purpose is to provide a dedicated, uniform, unambiguous representation of
relative time offsets, compatible with native Python functions such as
`time.sleep()` - but also with conversion methods to support additional common
representations such as:

  - `datetime.timedelta`
  - `google.protobuf.Duration`
  - Scaled integer durations (milliseconds/microseconds/nanoseconds)

### Usage Examples:

  ```
  >>> from cc.core.timeinterval import TimeInterval

  ### Create a new TimeInterval value from scalar components
  >>> ti = TimeInterval(seconds = 5, milliseconds = 500)

  ### This type extends `float`
  >>> isinstance(ti, float)
  True

  ### This type overrides the `__str__()` and `__repr__()` methods
  >>> ti
  '5.500s'
  >>> print("%s" % ti)
  5s
  >>> print("%d" % ti)
  5
  >>> print("%f" % ti)
  5.000000

  ### Also the type persists when adding/subtracting other time deltas
  >>> ti += 60
  >>> ti
  '1m 5s'
  >>> ti -= TimeInterval.from_milliseconds(500)
  >>> ti
  '1m 4.5s'

  ### Convert from `timedelta` instances
  >>> ti += TimeInterval.from_timedelta(datetime.timedelta(minutes=5))
  >>> ti
  `6m 5s`

  ### Convert from `google.protobuf.Duration` objects
  >>> from google.protobuf.duration_pb2 import Duration
  >>> ti += TimeInterval.from_protobuf(Duration(seconds=25))
  >>> ti
  `6m 30s`

  ### Return duration as scaled integers
  >>> ti.to_seconds()
  5
  >>> ti.to_milliseconds()
  5000
  >>> ti.to_microseconds()
  5000000
  >>> ti.to_nanoseconds()
  5000000000
  ```
'''

__docformat__ = 'javadoc en'
__author__ = 'Tor Slettnes'

import datetime
import re

TimeIntervalType = datetime.timedelta

NANOSECOND   = 1e-9
MICROSECOND  = 1e-6
MILLISECOND  = 1e-3
SECOND       = 1
MINUTE       = 60
HOUR         = 60 * MINUTE
DAY          = 24 * HOUR
WEEK         = DAY * 7
MONTH        = 30 * DAY
YEAR         = 365 * DAY
LEAP         = 4 * YEAR


try:
    from google.protobuf.duration_pb2 import Duration as ProtoBufDuration
except ImportError:
    ProtoBufDuration = None
else:
    TimeIntervalType |= ProtoBufDuration


class TimeInterval (float):
    '''
    Dedicated representation of relative time intervals.

    This extends and can be used as a drop-in replacement/wrapper for
    Python-native `float` values, for instance as input as input for functions
    like `time.sleep()`.  At the same time, methods are provided for conversion
    to/from other relative time representations, including:

      - `datetime.timedelta`
      - `google.protobuf.Duration`
      - Scaled integer values (milliseconds/microseconds/nanoseconds)
    '''


    def __new__(cls,
                seconds: int|float|None = None,
                *,
                milliseconds: int|float|None = None,
                microseconds: int|float|None = None,
                nanoseconds: int|float|None = None,
                minutes: int|float|None = None,
                hours: int|float|None = None,
                days: int|float|None = None,
                weeks: int|float|None = None):

        scaled_inputs = [
            ("seconds",      seconds,      SECOND),
            ("milliseconds", milliseconds, MILLISECOND),
            ("microseconds", microseconds, MICROSECOND),
            ("nanoseconds",  nanoseconds,  NANOSECOND),
            ("minutes",      minutes,      MINUTE),
            ("hours",        hours,        HOUR),
            ("days",         days,         DAY),
            ("weeks",        weeks,        WEEK),
        ]

        duration = 0.0
        for (argument, value, scale) in  scaled_inputs:
            if isinstance(value, int|float):
                duration += (value*scale)

            elif value is not None:
                raise TypeError("Invalid type for '%s' argument: %s"%
                                (argument, type(value).__name__,))

        return super().__new__(cls, duration)


    def __str__(self):
        '''
        Return an informal string respresentation of this duration
        '''
        return self.to_string()

    def __repr__(self):
        '''
        Return a formal string represenation of this duration
        '''
        return f"'{self}'"

    def __add__ (self, other: TimeIntervalType|str|float|int):
        '''
        Add another relative time interval to this one
        '''

        return TimeInterval(float(self) + TimeInterval.from_value(other))


    def __sub__(self, other: TimeIntervalType|str|float|int):
        '''
        Subtract another relative time interval from this one
        '''

        return TimeInterval(float(self) - TimeInterval.from_value(other))


    @classmethod
    def try_from(cls,
                 input: TimeIntervalType,
                 fallback: object|None = None,
                 decimal_exponent: int = 0,
                 ) -> 'TimeInterval':
        '''
        Create a new TimeInterval from a variant/undetermined type, such as an
        uncontrolled JSON or YAML settings value.

        If this fails, try again with the fallback value if provided. If the
        fallback is None or otherwise could not be converted to a TimeInterval, it
        is returned unmodified.  In any case, no exception is raised.

        See `from_value()` for a description of the remaining input
        arguments and other details.
        '''

        try:
            return cls.from_value(input, decimal_exponent)
        except (ValueError, TypeError):
            pass

        if fallback is not None:
            try:
                return cls.from_value(fallback, decimal_exponent)
            except (ValueError, TypeError):
                pass

        return fallback


    @classmethod
    def from_value(cls,
                   input: TimeIntervalType|float|str|int,
                   decimal_exponent: int = 0,
                   ) -> 'TimeInterval':
        '''
        Create a new TimeInterval value from any supported time interval representation.

        * If the input is an existing `TimeInterval` value, it is returned intact.

        * Any other `int`, `float`, or string representation of a plain number
          is scaled to seconds according to the `decimal_exponent` input argument.

        * Any `datetime.timedelta` input is passed on to `from_timedelta()`.

        * Any `google.protobuf.TimeInterval` object is passed on to `from_protobuf()`.

        * An annotated string, as returned by `to_string()` or `to_json_string()`.

        * Any other input type raises a TypeError.


        Input Arguments:

        @param input
            A supported time interval representation

        @param decimal_exponent
            Any numeric input is multiplied by ten to the specified power to
            yield seconds. For instance, `0` means no scaling, `-3` means
            convert from milliseconcds.

        @returns
            A new `TimeInterval` value.

        @exception ValueError
            The input is a string but could not be converted to `TimeInterval`.

        @exception TypeError
            The input type is not a supported time interval representation.
        '''

        if isinstance(input, TimeInterval):
            return input

        if isinstance(input, str):
            try:
                input = float(input)
            except ValueError:
                pass

        if isinstance(input, int|float):
            return TimeInterval(input * (10**decimal_exponent))

        elif isinstance(input, datetime.timedelta):
            return TimeInterval(input.total_seconds())

        elif ProtoBufDuration and isinstance(input, ProtoBufDuration):
            return TimeInterval(input.ToNanoseconds() / 1e9)

        elif isinstance(input, str):
            return TimeInterval.from_string(input)

        else:
            raise TypeError("Could not convert %s object to TimeInterval"%
                            (type(input).__name__,))


    @classmethod
    def from_seconds(cls, seconds: int|float) -> 'TimeInterval':
        '''
        Return a new TimeInterval value given number of seconds.

        This is effectively the same as `TimeInterval(seconds)`, provided for completeness.
        '''
        return TimeInterval(seconds)


    @classmethod
    def from_milliseconds(cls, milliseconds: int|float) -> 'TimeInterval':
        '''
        Return a new TimeInterval value given number of milliseconds.
        '''
        return TimeInterval(milliseconds / 1e3)


    @classmethod
    def from_microseconds(cls, microseconds: int|float) -> 'TimeInterval':
        '''
        Return a new TimeInterval value given number of microseconds
        '''
        return TimeInterval(microseconds / 1e6)


    @classmethod
    def from_nanoseconds(cls, nanoseconds: int|float) -> 'TimeInterval':
        '''
        Return a new TimeInterval value given number of nanoseconds
        '''
        return TimeInterval(nanoseconds / 1e9)


    _rx_component = re.compile(r'^([+-]?)([0-9\.]+)([a-z]+)$')

    @classmethod
    def from_string(cls,
                    input              : str,
                    years_suffix       : str|None = "y",
                    months_suffix      : str|None = None,
                    weeks_suffix       : str|None = "w",
                    days_suffix        : str|None = "d",
                    hours_suffix       : str|None = "h",
                    minutes_suffix     : str|None = "m",
                    seconds_suffix     : str|None = "s",
                    milliseconds_suffix: str|None = "ms",
                    microseconds_suffix: str|None = "us",
                    nanoseconds_suffix : str|None = "ns",
                    component_separator: str|None = None,
                    ) -> 'TimeInterval':

        components = {
            years_suffix: YEAR,
            months_suffix: MONTH,
            weeks_suffix: WEEK,
            days_suffix: DAY,
            hours_suffix: HOUR,
            minutes_suffix: MINUTE,
            seconds_suffix: SECOND,
            milliseconds_suffix: MILLISECOND,
            microseconds_suffix: MICROSECOND,
            nanoseconds_suffix: NANOSECOND,
        }

        negative = None
        total    = 0

        for component in input.split(component_separator):
            if m := cls._rx_component.match(component.strip()):
                sign, number, suffix = m.groups()
                if negative is None:
                    negative = (sign == '-')

                if scale := components.get(suffix):
                    total += scale * float(number)

                else:
                    raise ValueError("Invalid unit suffix following %s%s: %r"%(sign, number, suffix))

            else:
                raise ValueError('Invalid duration component: ' + component)


        if total > LEAP:
            ### For durations > 4 years we add leap days
            total += DAY * (total//LEAP)

        return TimeInterval(-total if negative
                            else total)


    @classmethod
    def from_timedelta(cls,
                       input: datetime.timedelta) -> 'TimeInterval':
        '''
        Create a new TimeInterval object from a `datetime.timedelta` value.

        @param input
            An existing `datetime.timedelta` value.
        '''

        return TimeInterval(input.total_seconds())


    @classmethod
    def from_protobuf(cls,
                      input: ProtoBufDuration):
        '''
        Create a new TimeInterval from a `google.protobuf.Duration` object.

        @param input
            An existing instance of `google.protobuf.Duration`
        '''

        return TimeInterval(input.ToNanoseconds() / 1e9)


    def to_seconds(self) -> int:
        '''
        Return this duration as an integer denoting seconds
        '''
        return int(self)


    def to_milliseconds(self) -> int:
        '''
        Return this duration as an integer denoting milliseconds
        '''
        return int(self * 1000)


    def to_microseconds(self) -> int:
        '''
        Return this duration as an integer denoting microseconds
        '''
        return int(self * 1e6)


    def to_nanoseconds(self) -> int:
        '''
        Return this duration as an integer denoting nanoseconds
        '''
        return int(self * 1e9)


    def to_json_string(self) -> str:
        '''
        Return a string representing this duration as seconds followed by a
        literal "s".

        The string may have 0, 3, 6 or 9 fractional digits, depending on the
        sub-second portion of the value.  The smallest representable time
        interval is one nanosecond.

        This method is a convenience wrapper for `to_string()`. The name
        `to_json_string()` as well as the output format is modeled after the
        similarly-named `google.protobuf.Duration()` method `ToJsonString()`.
        '''

        return self.to_string(
            years_suffix = None,
            months_suffix = None,
            weeks_suffix = None,
            days_suffix = None,
            hours_suffix = None,
            minutes_suffix = None,
            seconds_suffix = "s")

        # string = "%.9f"%(self,)
        # while string.endswith('000'):
        #     string = string[:-3]
        # return string.rstrip(".,") + 's'


    def to_string(self, *,
                  years_suffix       : str|None = "y",
                  months_suffix      : str|None = None,
                  weeks_suffix       : str|None = None,
                  days_suffix        : str|None = "d",
                  hours_suffix       : str|None = "h",
                  minutes_suffix     : str|None = "m",
                  seconds_suffix     : str|None = "s",
                  component_separator: str = " ",
                  positive_sign      : str = "",
                  negative_sign      : str = "-",
                  decimals           : int|None = None) -> str:
        '''
        Convert this duration to a string broken up into one or more
        components, representing date/time divisions from largest to smallest.

        Each component comprises a number followed by a time unit suffix,
        for instance, `"42d"` to represent 42 days. It is included only if
         - the corresponding `TIMEUNIT_suffix` argument is a non-empty string,
         - and the remainder after subtracting prior components is nonzero.

        The leading component will be prefixed with `positive_sign` or
        `negative_sign` if the duration is greater than or less than zero,
        respectively.

        The numeric values for all but the final component are integers. The
        final component, seconds, may contain fractional digits as follows:
         - If `decimals` is specified, that many fractional digits is added.
         - Otherwise, the string may have 0, 3, 6, or 9 fractional digits,
           depending on the sub-second portion of the value.  The smallest
           representable time interval in this case is one nanosecond.

        Components are separated by `component_separator` - by default, a single
        space character.

        The default output format matches the default expected input format for
        the `from_seconds()` method.  If you pass in alternate values for one or
        more `_suffix` arguments, you should do the same if/when invoking that
        method.

        See also the simpler `to_json_string()`, which produces only a seconds
        component.
        '''

        components = [
            (years_suffix, YEAR),
            (months_suffix, MONTH),
            (weeks_suffix, WEEK),
            (days_suffix, DAY),
            (hours_suffix, HOUR),
            (minutes_suffix, MINUTE),
        ]

        sign = (positive_sign if (self > 0)
                else "-" if (self < 0)
                else '')

        remainder = (self if (self >= 0)
                     else -self)

        if remainder > LEAP:
            ### For durations > 4 years we subtract leap days
            remainder -= DAY * (remainder//LEAP)

        parts = []

        for (suffix, division) in components:
            if suffix and (remainder >= division):
                count = remainder // division
                parts.append("%d%s"%(count, suffix))
                remainder %= division

        if seconds_suffix and (remainder or not parts):
            if decimals:
                parts.append("%.*f%s"%(decimals, remainder, seconds_suffix))

            else:
                string = "%.9f"%(remainder,)
                while string.endswith('000'):
                    string = string[:-3]
                parts.append(string.rstrip(".,") + seconds_suffix)

        return sign + component_separator.join(parts)


    def to_timedelta(self) -> datetime.timedelta:
        '''
        Convert this duration to a `datetime.timedelta` object.

        @return
            A new `datetime.timedelta` instance.
        '''

        return datetime.timedelta(seconds=self)


    def to_protobuf(self) -> ProtoBufDuration:
        '''
        Convert this duration to the well-known ProtoBuf type
        `google.protobuf.Duration`.

        @return
             New `google.protobuf.Duration` instance.
        '''
        from google.protobuf.duration_pb2 import Duration
        return Duration(
            seconds = int(self),
            nanos = int((self - int(self)) * 1e9))


TimeIntervalType |= TimeInterval
