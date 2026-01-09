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
  >>> from cc.core.timeutils import TimeInterval

  ### Create a new TimeInterval value from scalar components
  >>> ti = TimeInterval(seconds = 5, milliseconds = 500)

  ### This type extends `float`
  >>> isinstance(ti, float)
  True

  ### This type overrides the `__str__()` and `__repr__()` methods
  >>> ti
  TimeInterval(5.5s)
  >>> print("%s" % ti)
  5.5s
  >>> print("%d" % ti)
  5
  >>> print("%f" % ti)
  5.500000

  ### Also the type persists when adding/subtracting other time deltas
  >>> ti += 60
  >>> ti
  '1m 5s'
  >>> ti -= TimeInterval.from_milliseconds(500)
  >>> ti
  '1m 4.5s'

  ### Convert from `timedelta` instances
  >>> ti += TimeInterval.from_value(datetime.timedelta(minutes=5))
  >>> ti
  `6m 5s`

  ### Convert from `google.protobuf.Duration` objects
  >>> from google.protobuf.duration_pb2 import Duration
  >>> ti += TimeInterval.from_value(Duration(seconds=25))
  >>> ti
  `6m 30s`


  ### Convert from ISO 8601 Duration strings
  >>> TimeInterval.from_value("P3Y2M1DT23H59M59.5S")
  '3y 61d 23h 59m 59.500s'

  >>> TimeInterval.from_value("P


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
        return "TimeInterval(%s)"%(
            self.to_string(component_separator=", "),
        )

    def __add__ (self, other: TimeIntervalType|str|float|int) -> 'TimeInterval':
        '''
        Add another relative time interval to this one
        '''

        return TimeInterval(float(self) + TimeInterval.from_value(other))


    def __sub__(self, other: TimeIntervalType|str|float|int) -> 'TimeInterval':
        '''
        Subtract another relative time interval from this one
        '''

        return TimeInterval(float(self) - TimeInterval.from_value(other))

    def __mul__ (self, scalar: float):
        '''
        Multiply this time inteval by a scalar value
        '''
        return TimeInterval(float(self) * scalar)

    def __truediv__ (self, divisor: float):
        '''
        Divide this time inteval by a scalar value
        '''
        return TimeInterval(float(self) / divisor)

    @classmethod
    def try_from(cls,
                 input: TimeIntervalType|float|int|str,
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
                   input: TimeIntervalType|float|int|str,
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


    _rx_duration = re.compile(
        "^([+-])?"                         # (1) +/-
        "(?:P"                            # DATE:
        "\\s*(?:(\\d+(?:\\.\\d*)?)[yY])?" # (2) Years
        "\\s*(?:(\\d+(?:\\.\\d*)?)[mM])?" # (3) Months
        "\\s*(?:(\\d+(?:\\.\\d*)?)[wW])?" # (4) Weeks
        "\\s*(?:(\\d+(?:\\.\\d*)?)[dD])?" # (5) Days
        ")?(?:\\s*T?"                     # TIME:
        "\\s*(?:(\\d+(?:\\.\\d*)?)[hH])?" # (6) Hours
        "\\s*(?:(\\d+(?:\\.\\d*)?)[mM])?" # (7) Minutes
        "\\s*(?:(\\d+(?:\\.\\d*)?)[sS])?" # (8) Seconds
        "\\s*(?:(\\d+(?:\\.\\d*)?)ms)?"   # (9) Milliseconds
        "\\s*(?:(\\d+(?:\\.\\d*)?)us)?"   # (10) Microseconds
        "\\s*(?:(\\d+(?:\\.\\d*)?)ns)?"   # (11) Nanoseconds
        ")$")


    _unit_map = {
        2: YEAR,
        3: MONTH,
        4: WEEK,
        5: DAY,
        6: HOUR,
        7: MINUTE,
        8: SECOND,
        9: MILLISECOND,
        10: MICROSECOND,
        11: NANOSECOND,
    }

    @classmethod
    def from_string(cls, input: str) -> 'TimeInterval':
        interval = 0.0
        hit = False

        if m := cls._rx_duration.match(input):

            for group_index, unit_factor in cls._unit_map.items():
                if word := m.group(group_index):
                    interval += unit_factor * float(word)
                    hit = True

            # Approximate leap years by adding an extra day for every 4 years
            if years := m.group(2):
                interval += DAY * (float(years) // LEAP)

            if m.group(1) == "-":
                interval = -interval

        if hit:
            return TimeInterval(interval)
        else:
            raise ValueError(
                "Invalid duration: %s"%(
                    input,
                ))


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


    def to_iso8601_string(self) -> str:
        '''
        Return a ISO 8601 compliant string representing this duration.

        The string may have 0, 3, 6 or 9 fractional digits, depending on the
        sub-second portion of the value.  The smallest representable time
        interval is one nanosecond.

        This method is a convenience wrapper for `to_string()`.
        '''

        return self.to_string(
            years_suffix = "Y",
            months_suffix = None,
            weeks_suffix = None,
            days_suffix = "D",
            hours_suffix = "H",
            minutes_suffix = "M",
            seconds_suffix = "S",
            component_separator = "",
            preamble = "P",
            date_time_separator = "T"
        )


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
            seconds_suffix = "s",
        )

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
                  decimals           : int|None = None,
                  preamble           : str = "",
                  date_time_separator: str = "") -> str:
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

        date_components = [
            (years_suffix, YEAR),
            (months_suffix, MONTH),
            (weeks_suffix, WEEK),
            (days_suffix, DAY),
        ]

        time_components = [
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
        for (prefix, components) in [("", date_components),
                                     (date_time_separator, time_components)]:

            if prefix and (remainder or not parts):
                parts.append(prefix)

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

        return sign + preamble + component_separator.join(parts)


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
