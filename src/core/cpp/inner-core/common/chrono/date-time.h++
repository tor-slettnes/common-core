/// -*- c++ -*-
//==============================================================================
/// @file date-time.h++
/// @brief Date/time utilities
/// @author Tor Slettnes <tor@slett.net>
///
/// Convenience defintions for time representations using std::chrono::system_clock.
/// For instance, to convert a C "struct timespec" instance to a time_point object,
/// falling back to the current time if the time is zero (epoch), use
///     dt::TimePoint tp = dt::to_timepoint(ts, dt::Clock::now());
//==============================================================================

#pragma once
#include <chrono>
#include <ctime>
#include <string>
#include <optional>
#include <ostream>

using namespace std::literals::chrono_literals;

namespace core
{
    /// Date/Time utilities
    namespace dt
    {
        // constexpr auto ISO_FORMAT = "%F %T";
        constexpr auto DEFAULT_FORMAT = "%F@%T";
        constexpr auto DEFAULT_TIME_FORMAT = "%T";
        constexpr auto DEFAULT_DURATION_FORMAT = "%H:%M:%S";
        constexpr auto JS_FORMAT = "%FT%T";

        // Lower limit for interpreting scalar timestamps as nanoseconds since
        // epoch.  Scalar inputs with unspecified precision are repeatedly
        // multiplied by 1000 until they exceed this limit.
        constexpr long long EPOCH_NANOS_LOWER_LIMIT = 1e17;

        // typedef std::chrono::system_clock Clock;
        // typedef Clock::duration Duration;
        // typedef Clock::time_point TimePoint;
        using Clock = std::chrono::system_clock;
        using Duration = Clock::duration;
        using TimePoint = Clock::time_point;

        // using Duration = std::chrono::duration<long int, std::nano>;
        // using TimePoint = std::chrono::time_point<Clock, Duration>;

        extern const TimePoint epoch;

        struct TimeZoneInfo
        {
            std::string shortname;  // Abreviation, e.g. PST or PDT
            Duration offset;        // Offset from UTC, e.g. -7*60*60
            Duration stdoffset;     // Standard offset from UTC, e.g. -8*60*60
            bool dst = false;       // Whether daylight savings time is in effect
        };

        std::ostream &operator<<(std::ostream &stream, const TimeZoneInfo &zi);

        constexpr auto TM_YEAR_OFFSET = 1900;
        constexpr auto TM_MONTH_OFFSET = 1;
        constexpr auto TM_DAY_OFFSET = 0;
        constexpr auto TM_YEARDAY_OFFSET = 1;
        constexpr auto TM_WEEKDAY_OFFSET = 0;

        enum class TimeUnit
        {
            ZERO_TIME,    // No/Unspecified time
            NANOSECOND,   // 1e-9 second
            MICROSECOND,  // 1e-6 second
            MILLISECOND,  // 1e-3 second
            SECOND,       // SI time unit
            MINUTE,       // 60 seconds
            HOUR,         // 60 minutes
            DAY,          // 24 hours, approx 1 Earth rotation towards Sun
            WEEK,         // 7 days
            MONTH,        // 28-31 days (variable)
            YEAR,         // 12 months (variable)
            ETERNITY      // Inifinite time
        };

        std::ostream &operator<<(std::ostream &stream, const TimeUnit &unit);
        std::istream &operator>>(std::istream &stream, TimeUnit &unit);

        struct DateTimeInterval
        {
            operator bool() const noexcept;
            void reset();
            std::optional<Duration> as_duration() const;
            Duration as_approximate_duration() const;

            TimeUnit unit = TimeUnit::ZERO_TIME;
            std::int64_t count = 0;
        };

        std::ostream &operator<<(std::ostream &stream, const DateTimeInterval &interval);
        std::istream &operator>>(std::istream &stream, DateTimeInterval &interval);

        /// Write out provided timepoint as a string representing UTC or local time.
        /// @param[in] stream
        ///     Output stream
        /// @param[in] tp
        ///     A time point
        /// @param[in] local
        ///     Represent in the local timezone; otherwise UTC
        /// @param[in] decimals
        ///     Sub-second resolution. The default is 3 (i.e., milliseconds)
        /// @param[in] format
        ///     Output format for std::put_time(). The default is `%F@%T`,
        ///     yielding an ISO 8601 string (`YYYY-MM-DD@HH:MM:SS`)
        void tp_to_stream(std::ostream &stream,
                          const TimePoint &tp,
                          bool local = true,
                          uint decimals = 3,
                          const std::string &format = DEFAULT_FORMAT);

        /// Write out formatted string representation of the provided duration.
        /// @param[in] stream
        ///     Output stream
        /// @param[in] dur
        ///     A duration
        /// @param[in] decimals
        ///     Sub-second resolution. The default is 3 (i.e., milliseconds)
        /// @param[in] format
        ///     Output format for std::put_time().
        void dur_to_stream(std::ostream &stream,
                           const Duration &dur,
                           uint decimals,
                           const std::string &format = DEFAULT_DURATION_FORMAT);

        /// Write out a human readable string representation of the provided
        /// duration, e.g., `42 hours, 12 minutes, 2.322 seconds`.
        /// @param[in] stream
        ///     Output stream
        /// @param[in] dur
        ///     A duration
        /// @param[in] secondsformat
        ///     Seconds representation. Always presente (unless empty).
        /// @param[in] minutesformat
        ///     Minutes representation for durations at least one minute.
        /// @param[in] hoursformat
        ///     Hours representation for durations at least one hour.
        /// @param[in] daysformat
        ///     Days representation for durations at least one day.
        /// @param[in] monthsformat
        ///     Representation for number of 30-day cycles in duration.
        /// @param[in] yearsformat
        ///     Representation for number of 365-day cycles in duration.
        /// @param[in] max_divisions
        ///     Maximum number of time units to include
        /// @param[in] delimiter
        ///     Delimiter between time units
        ///
        /// Format strings are printf() style, interpreted by `str::format()`.
        ///
        /// Each time unit is represented modulo any larger time unit for which a
        /// format string is provided (even if empty).  For instance if
        /// `yearsformat` is provided but `monthsformat` is not, the number of days
        /// is shown modulo 365.

        void dur_to_stream(std::ostream &stream,
                           const Duration &dur,
                           const std::optional<std::string> &secondsformat = "%.3gs",
                           const std::optional<std::string> &minutesformat = "%zm",
                           const std::optional<std::string> &hoursformat = "%zh",
                           const std::optional<std::string> &daysformat = "%zd",
                           const std::optional<std::string> &monthsformat = {},
                           const std::optional<std::string> &yearsformat = "%zy",
                           uint max_divisions = 3,
                           const std::string delimiter = " ");

        /// Return the provided timepoint as a JavaScript time string
        /// @param[in] tp
        ///     A timepoint
        /// @return
        ///     Timepoint represented as a JavaScript time string
        std::string to_js_string(const TimePoint &tp);

        /// Return the provided timepoint as a string representing UTC or local time.
        /// @param[in] tp
        ///     A timepoint
        /// @param[in] local
        ///     Represent in the local timezone; otherwise UTC
        /// @param[in] decimals
        ///     Sub-second resolution. The default is 3 (i.e., milliseconds)
        /// @param[in] format
        ///     Output format for std::put_time(). The default is `%F@%T`,
        ///     yielding an ISO 8601 string (`YYYY-MM-DD@HH:MM:SS`)
        /// @return
        ///     String representing the provided timeppoint as UTC or local time.
        std::string to_string(const TimePoint &tp,
                              bool local,
                              uint decimals = 3,
                              const std::string &format = DEFAULT_FORMAT);

        /// Return the provided timepoint as a string.
        /// representing local time.
        /// @param[in] tp
        ///     A timepoint
        /// @param[in] decimals
        ///     Sub-second resolution. The default is 3 (i.e., milliseconds)
        /// @param[in] format
        ///     Output format for std::put_time(). The default is `%F@%T`,
        ///     yielding an ISO 8601 string (`YYYY-MM-DD@HH:MM:SS`)
        /// @return
        ///     String representing the provided timeppoint as local time.
        std::string to_string(const TimePoint &tp,
                              uint decimals = 3,
                              const std::string &format = DEFAULT_FORMAT);

        /// Return a formatted string representation of the provided duration.
        /// @param[in] duration
        ///    A duration
        /// @param[in] decimals
        ///     Sub-second resolution.
        /// @param[in] format
        ///     Output format for std::put_time().
        /// @return
        ///     String representing the provided duration
        std::string to_string(const Duration &duration,
                              uint decimals,
                              const std::string &format = DEFAULT_DURATION_FORMAT);

        /// @brief
        ///     Return a human readable string representation of the provided duration,
        ///     e.g., `42 hours, 12 minutes, 2.322 seconds`.
        /// @param[in] duration
        ///     A duration
        /// @param[in] secondsformat
        ///     Seconds representation. Always presente (unless empty).
        /// @param[in] minutesformat
        ///     Minutes representation for durations at least one minute.
        /// @param[in] hoursformat
        ///     Hours representation for durations at least one hour.
        /// @param[in] daysformat
        ///     Days representation for durations at least one day.
        /// @param[in] monthsformat
        ///     Representation for number of 30-day cycles in duration.
        /// @param[in] yearsformat
        ///     Representation for number of 365-day cycles in duration.
        /// @param[in] max_divisions
        ///     Maximum number of time units to include
        /// @param[in] delimiter
        ///     Delimiter between time units
        /// @return
        ///     String representing the provided duration
        ///
        /// Format strings are printf() style, interpreted by `str::format()`.
        ///
        /// Each time unit is represented modulo any larger time unit for which a
        /// format string is provided (even if empty).  For instance if
        /// `yearsformat` is provided but `monthsformat` is not, the number of days
        /// is shown modulo 365.

        std::string to_string(const Duration &duration,
                              const std::optional<std::string> &secondsformat = "%.3gs",
                              const std::optional<std::string> &minutesformat = "%zm",
                              const std::optional<std::string> &hoursformat = "%zh",
                              const std::optional<std::string> &daysformat = "%zd",
                              const std::optional<std::string> &monthsformat = {},
                              const std::optional<std::string> &yearsformat = "%zy",
                              uint max_divisions = 6,
                              const std::string &delimiter = " ");

        /// Return a human readable string representation of the provided `std::tm` structure.
        std::string to_string(const std::tm &tm,
                              const std::string &format = DEFAULT_FORMAT);

        /// Convert from TimePoint (std::chrono::system_clock__time_point) to C "struct timespec";
        timespec to_timespec(const TimePoint &tp);

        /// Convert the provided timepoint to the `time_t` C type (seconds since epoch).
        std::time_t to_time_t(const TimePoint &tp);

        /// Convert a duration to seconds (truncated, not rounded)
        std::int64_t to_seconds(const Duration &d);

        /// Convert a duration to milliseconds (truncated, not rounded) since UNIX epoch
        std::int64_t to_milliseconds(const TimePoint &tp);

        /// Convert a duration to milliseconds (truncated, not rounded)
        std::int64_t to_milliseconds(const Duration &d);

        template <class Scale>
        std::int64_t to_scalar(const TimePoint &tp)
        {
            return to_scalar<Scale>(tp.time_since_epoch());
        }

        template <class Scale>
        std::int64_t to_scalar(const Duration &dur)
        {
            return std::chrono::duration_cast<Scale>(dur).count();
        }

        /// Convert from TimePoint (std::chrono::system_clock::time_point) to double
        double to_double(const TimePoint &tp);

        /// Convert from Duration (std::chrono::system_clock::duration) to double
        double to_double(const Duration &d);

        //--------------------------------------------------------------------------
        // Duration conversions

        /// Convert from milliseconds (Java style) to Duration
        Duration ms_to_duration(std::int64_t milliseconds);

        /// Convert from seconds and nanoseconds as integers to Duration
        Duration to_duration(time_t seconds, long nanoseconds);

        /// Convert from a scalar value to Duration
        /// @param[in] scalar
        ///     Input value
        /// @param[in] multiplier
        ///     Number of seconds per time unit, e.g., 0.001 to represent milliseconds.
        Duration to_duration(double scalar, double multiplier);

        // Convert from seconds to Duration
        Duration to_duration(double seconds);

        /// Convert from `struct timespec` to Duration
        Duration to_duration(const timespec &ts);

        /// Convert from string representation (`HH:MM:SS.sss`) to Duration
        Duration to_duration(
            const std::string_view &text,
            const std::optional<std::string> &format = {},
            const Duration fallback = Duration::zero());

        std::optional<Duration> try_to_duration(
            const std::string_view &text);

        // Try to convert from a string to a duration
        std::optional<Duration> try_to_duration(
            const std::string_view &text,
            const std::string &format);

        //--------------------------------------------------------------------------
        // Timepoint conversions

        /// Convert a scalar epoch-based timestamp a TimePoint.
        ///
        /// @param[in] scalar
        ///     Time units since UNIX Epoch
        ///
        /// @param[in] multiplier_decimal_expoent
        ///     Decimal exponent yielding the number of seconds per time unit,
        ///     e.g. -9 for nanoseconds, -3 for millseconds or 0 for seconds.
        ///
        /// If `multiplier_decimal_exponent` is absent, the scalar value is
        /// repeatedly divided by 1000 until it falls below 1e11
        /// (100,000,000,000), corresponding to November 16 in the year 5138
        /// with a lower bound of March 3, 1973.

        TimePoint double_to_timepoint(
            double value,
            const std::optional<int> &multiplier_decimal_exponent = {});

        /// Convert a scalar epoch-based timestamp of specific precision
        /// precision to a TimePoint.
        ///
        /// @param[in] scalar
        ///     Time units since UNIX Epoch
        ///
        /// @param[in] multiplier_decimal_exponent
        ///     Decimal exponent yielding the number of seconds per time unit,
        ///     e.g., -3 for milliseconds or 0 for seconds.

        TimePoint int_to_timepoint(
            std::int64_t scalar,
            int multiplier_decimal_exponent);

        /// Convert a scalar epoch-based timestamp of arbitrary/unknown
        /// precision to a TimePoint.
        ///
        /// @param[in] scalar
        ///     Time units since UNIX Epoch
        ///
        /// The scalar value is first converted to nanoseconds since epoch by
        /// repeatedly multiplied 1000 until it exceeds EPOCH_NANOS_LOWER_LIMIT
        /// (1e17), then converted to a `TimePoint`. The resulting timepoint is
        /// bounded between March 3, 1973, and November 16, 5138.

        TimePoint int_to_timepoint(
            std::int64_t scalar);

        /// Convert a scalar epoch-based timestamp with optional precision
        /// to a TimePoint.
        ///
        /// @param[in] scalar
        ///     Time units since UNIX Epoch
        ///
        /// @param[in] multiplier_decimal_exponent
        ///     Decimal exponent yielding the number of seconds per time unit,
        ///     e.g., -3 for milliseconds or 0 for seconds.
        ///
        /// If `multiplier_decimal_exponent` contains no value, the precision
        /// is dynamically determined as described above.

        TimePoint int_to_timepoint(
            std::int64_t scalar,
            const std::optional<int> &multiplier_decimal_exponent);

        /// Convert from milliseconds (Java style timestamp) to TimePoint
        TimePoint ms_to_timepoint(std::int64_t milliseconds);

        /// Convert from seconds and nanoseconds to TimePoint, or fallback if the time is zero.
        TimePoint to_timepoint(time_t seconds, long nanoseconds = 0);

        /// Convert from "struct timespec" to TimePoint, or fallback if the time is zero.
        TimePoint to_timepoint(const timespec &ts);

        /// Convert from "struct tm" to TimePoint, or fallback if the time is zero.
        TimePoint to_timepoint(const tm &dt, bool local = true);

        /// @brief
        ///    Convert from year/month/day/hour/minute/second/fraction to timepoint.
        /// @param[in] year
        ///    Year
        /// @param[in] month
        ///    Month within the year
        /// @param[in] day
        ///    Day within the month
        /// @param[in] hour
        ///    Hour within the day
        /// @param[in] minute
        ///    Minute within the hour
        /// @param[in] second
        ///    Seconds within the minue
        /// @param[in] fraction
        ///    Additional (presumably sub-second) time.
        /// @param[in] tz_offset
        ///    Timezone offset if any.  If not provided use current local timezone offset.
        ///    Use 0 for UTC.
        TimePoint to_timepoint(std::int32_t year,
                               std::uint32_t month,
                               std::uint32_t day,
                               std::uint32_t hour,
                               std::uint32_t minute,
                               std::uint32_t second,
                               double fraction,
                               std::optional<Duration> tz_offset);

        TimePoint to_timepoint(
            const std::string_view &s,
            bool assume_local = true,
            const TimePoint &fallback = {},
            const std::optional<int> multiplier_decimal_exponent = {});

        TimePoint to_timepoint(
            const std::string_view &s,
            const std::string &format,
            bool assume_local = true,
            const TimePoint &fallback = {});

        // @brief
        //    Convert a ISO-formatted string (yyyy-mm-dd?hh:mm:ss[.fraction][Z]) to a timepoint.
        // @param[in] input
        //    ISO 10646 formatted input string.
        // @param[in] assume_local
        //    Assume local time if the trailing `Z'` is missing.

        std::optional<TimePoint> try_to_timepoint(
            const std::string_view &input,
            bool assume_local = true,
            const std::optional<int> multiplier_decimal_exponent = {});

        // @brief
        //    Convert a string with custom formatting to a timepiont.
        // @param[in] input
        //    Time string formatted according to `format`
        // @param[in] format
        //    Conversion Format.  See `man strptime` for details.
        // @param[in] assume_local
        //    Assume local time unless the input string ends in `Z`.

        std::optional<TimePoint> try_to_timepoint(
            const std::string_view &input,
            const std::string &format,
            bool assume_local = true);

        //--------------------------------------------------------------------------
        // Zone alignment and conversions

        /// Return the most recent midnight prior to a given timestamp,
        /// in either the local timezone or UTC.
        TimePoint last_midnight(const TimePoint &tp = Clock::now(), bool local = true);

        /// Return the most recent time aligned to the specific date/time interval
        TimePoint last_aligned(const TimePoint &tp,
                               const DateTimeInterval &interval,
                               bool local = true);

        /// Return the most recent time aligned to the specific clock interval
        TimePoint last_aligned(const TimePoint &tp,
                               const Duration &interval,
                               bool local = true);

        /// Return the most recent time aligned to a specific reference (e.g. epoch)
        /// and interval
        TimePoint last_aligned(const TimePoint &tp,
                               const TimePoint &reference,
                               const Duration &interval);

        /// Return the local timezone offset. Note that this will be affected by the
        /// provided timepoint wherever daylight savings time/summer time is used.
        Duration local_adjustment(const TimePoint &tp);

        /// Return the local timezone offset. Note that this will be affected by the
        /// provided timepoint wherever daylight savings time/summer time is used.
        Duration local_adjustment(const TimePoint &tp,
                                  const std::string &timezone);

        /// Convert the provided timepoint to a C `tm` structure representing UTC
        /// date/time.
        std::tm gmtime(const TimePoint &tp = Clock::now());

        /// Convert the provided time_t value to a C `tm` structure representing UTC date/time.
        std::tm gmtime(const std::time_t &time);

        /// Convert the provided timepoint to a C `tm` structure representing local date/time.
        /// param[in] tp
        ///    Time point
        /// param[in] timezone
        ///    A string representing the timezone in which to represent the provided time.  If
        ///    empty, and if the TZ environment variable is not set, the system's default timezone
        ///    is used.
        std::tm localtime(const TimePoint &tp, const std::string &timezone);
        std::tm localtime(const TimePoint &tp = Clock::now());

        /// Convert the provided time_t value to a C `tm` structure representing local date/time.
        std::tm localtime(const std::time_t &time, const std::string &timezone);
        std::tm localtime(const std::time_t &time);

        /// @brief Return information about the specified time zone.
        /// @param[in] timezone
        ///    A string representing the timezone in which to represent the provided time.  If
        ///    empty, and if the TZ environment variable is not set, the system's default timezone
        ///    is used.
        /// @param[in] tp
        ///    Time point
        /// @return
        ///    A TimeZoneInfo structure containing information about the specified zone, see above.
        TimeZoneInfo tzinfo(const std::string &timezone, const TimePoint &tp = Clock::now());
        TimeZoneInfo tzinfo(const TimePoint &tp = Clock::now());

        TimeZoneInfo tzinfo(const std::string &timezone, const std::time_t &time);
        TimeZoneInfo tzinfo(const std::time_t &time);

        /// Convert a `struct tm` object to seconds since epoch.
        /// This is similar to `std::mktime()`, but also supports UTC time.
        std::time_t mktime(std::tm datetime, bool local = true);

        /// Helper function to calculate number of calendar days
        std::uint32_t gregorian_days(const tm &datetime);

        /// Helper function to determine if a year is a leap year
        bool is_leap_year(uint year, bool gregorian = true);

        /// Helper function to calculate day number within a year
        std::uint32_t day_of_year(uint year, uint month, uint day, bool gregorian = true);

    }  // namespace dt

    /// Steady Time
    namespace steady
    {
        using Clock = std::chrono::steady_clock;
        using Duration = Clock::duration;
        using TimePoint = Clock::time_point;
        // using Duration = std::chrono::duration<long int, std::nano>;
        // using TimePoint = std::chrono::time_point<Clock, Duration>;
        extern const TimePoint epoch;

        void tp_to_stream(std::ostream &stream,
                          const TimePoint &stp,
                          uint decimals = 3);

        // Convert from System Clock to Steady Clock
        TimePoint to_timepoint(dt::TimePoint tp);
    }  // namespace steady

    template <class TargetClock, class SourceClock>
    typename TargetClock::time_point timepoint_cast(const typename SourceClock::time_point &source_tp)
    {
        typename SourceClock::duration relative_to_now =
            source_tp.time_since_epoch() -
            SourceClock::now().time_since_epoch();

        return TargetClock::now() +
               std::chrono::duration_cast<typename TargetClock::duration>(relative_to_now);
    }

}  // namespace core

namespace std::chrono
{
    std::ostream &operator<<(std::ostream &stream, const system_clock::time_point &tp);
    std::ostream &operator<<(std::ostream &stream, const system_clock::duration &dur);
    std::ostream &operator<<(std::ostream &stream, const steady_clock::time_point &stp);
}  // namespace std::chrono

// Compare `std::tm` structs.  This is actually an alias for `struct tm`,
// so we need to define it in the global namespace.
bool operator==(const std::tm &lhs, const std::tm &rhs);

/// Represent `struct tm` instances
std::ostream &operator<<(std::ostream &stream, const std::tm &tm);
