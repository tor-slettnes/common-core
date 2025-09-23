/// -*- c++ -*-
//==============================================================================
/// @file date-time.c++
/// @brief Time/clock utilities
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "date-time.h++"
#include "types/symbolmap.h++"
#include "string/format.h++"
#include "string/convert.h++"
#include "platform/timezone.h++"

#include <string.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

namespace core
{
    namespace dt
    {
        const TimePoint epoch;
        const uint MINUTE = 60;
        const uint HOUR = 60 * MINUTE;
        const uint DAY = 24 * HOUR;
        const uint MONTH = 30 * DAY;
        const uint YEAR = 365 * DAY;
        const uint LEAP = 4 * YEAR;

        core::types::SymbolMap<TimeUnit, std::multimap<TimeUnit, std::string>>
            time_unit_names = {
                {TimeUnit::ZERO_TIME, "zero time"},
                {TimeUnit::NANOSECOND, "nanosecond"},
                {TimeUnit::NANOSECOND, "nanoseconds"},
                {TimeUnit::NANOSECOND, "ns"},
                {TimeUnit::MICROSECOND, "microsecond"},
                {TimeUnit::MICROSECOND, "microseconds"},
                {TimeUnit::MICROSECOND, "us"},
                {TimeUnit::MILLISECOND, "millisecond"},
                {TimeUnit::MILLISECOND, "milliseconds"},
                {TimeUnit::MILLISECOND, "ms"},
                {TimeUnit::SECOND, "second"},
                {TimeUnit::SECOND, "seconds"},
                {TimeUnit::SECOND, "s"},
                {TimeUnit::MINUTE, "minute"},
                {TimeUnit::MINUTE, "minutes"},
                {TimeUnit::MINUTE, "m"},
                {TimeUnit::HOUR, "hour"},
                {TimeUnit::HOUR, "hours"},
                {TimeUnit::HOUR, "hourly"},
                {TimeUnit::HOUR, "h"},
                {TimeUnit::DAY, "day"},
                {TimeUnit::DAY, "days"},
                {TimeUnit::DAY, "daily"},
                {TimeUnit::WEEK, "week"},
                {TimeUnit::WEEK, "weeks"},
                {TimeUnit::WEEK, "weekly"},
                {TimeUnit::MONTH, "month"},
                {TimeUnit::MONTH, "months"},
                {TimeUnit::MONTH, "monthly"},
                {TimeUnit::YEAR, "year"},
                {TimeUnit::YEAR, "years"},
                {TimeUnit::YEAR, "yearly"},
                {TimeUnit::ETERNITY, "eternity"},
        };

        std::ostream &operator<<(std::ostream &stream, const TimeZoneInfo &zi)
        {
            str::format(stream,
                        "{shortname=%r, offset=%s, stdoffset=%s, dst=%b}",
                        zi.shortname,
                        zi.offset,
                        zi.stdoffset,
                        zi.dst);
            return stream;
        }

        std::ostream &operator<<(std::ostream &stream, const TimeUnit &unit)
        {
            return time_unit_names.to_stream(stream, unit);
        }

        std::istream &operator>>(std::istream &stream, TimeUnit &unit)
        {
            return time_unit_names.from_stream(stream,  // stream
                                               &unit,   // key
                                               {},      // fallback
                                               true,    // flag_unknown
                                               true);   // allow_partial
        }

        std::ostream &operator<<(std::ostream &stream, const DateTimeInterval &interval)
        {
            switch (interval.unit)
            {
            case TimeUnit::ZERO_TIME:
            case TimeUnit::ETERNITY:
                time_unit_names.to_stream(stream, interval.unit);
                break;

            default:
                stream << interval.count << " ";
                time_unit_names.to_stream(stream, interval.unit);
                if ((interval.count != 1) && (interval.count != -1))
                {
                    stream << "s";
                }
            }
            return stream;
        }

        std::istream &operator>>(std::istream &stream, DateTimeInterval &interval)
        {
            std::istream::pos_type start = stream.tellg();
            stream >> interval.count;

            if (stream.fail())
            {
                interval.count = 1;
                stream.clear();
                stream.seekg(start);
            }
            else
            {
                interval.unit = TimeUnit::SECOND;
            }

            if (!stream.eof())
            {
                stream >> interval.unit;
            }
            return stream;
        }

        DateTimeInterval::operator bool() const noexcept
        {
            return (this->unit == TimeUnit::ETERNITY) ||
                   ((this->unit != TimeUnit::ZERO_TIME) && (this->count != 0));
        }

        void DateTimeInterval::reset()
        {
            this->unit = TimeUnit::ZERO_TIME;
            this->count = 0;
        }

        std::optional<Duration> DateTimeInterval::as_duration() const
        {
            switch (this->unit)
            {
            case TimeUnit::ZERO_TIME:
                return Duration::zero();

            case TimeUnit::NANOSECOND:
                return std::chrono::nanoseconds(1);

            case TimeUnit::MICROSECOND:
                return std::chrono::microseconds(1);

            case TimeUnit::MILLISECOND:
                return std::chrono::milliseconds(1);

            case TimeUnit::SECOND:
                return std::chrono::seconds(1);

            case TimeUnit::MINUTE:
                return std::chrono::minutes(1);

            case TimeUnit::HOUR:
                return std::chrono::hours(1);

            default:
                return {};
            }
        }

        Duration DateTimeInterval::as_approximate_duration() const
        {
            if (auto duration = this->as_duration())
            {
                return duration.value();
            }
            else
            {
                switch (this->unit)
                {
                case TimeUnit::DAY:
                    return std::chrono::hours(24);

                case TimeUnit::WEEK:
                    return std::chrono::hours(24 * 7);

                case TimeUnit::MONTH:
                    return std::chrono::hours(24 * 30);

                case TimeUnit::YEAR:
                    return std::chrono::hours(24 * 365 + 8);

                case TimeUnit::ETERNITY:
                    return std::chrono::hours::max();

                default:
                    return Duration::zero();
                }
            }
        }

        //==========================================================================
        // Helper functions, used below.

        template <class Dur, class Ref>
        TimePoint last_aligned_tp(const TimePoint &tp, uint count)
        {
            auto ref = std::chrono::floor<Ref>(tp);

            if (count)
            {
                auto rel = tp - std::chrono::time_point_cast<Dur>(ref);
                auto rel_aligned = rel - (rel % Dur(count));
                return std::chrono::time_point_cast<Dur>(ref) + rel_aligned;
            }
            else
            {
                return std::chrono::time_point_cast<Dur>(ref);
            }
        }

        TimePoint last_aligned_dt(TimePoint tp,
                                  int years,
                                  int months,
                                  int days,
                                  int hours,
                                  int minutes,
                                  int seconds,
                                  bool local)
        {
            std::tm dt = local ? localtime(tp) : gmtime(tp);

            std::tm aligned = {
                .tm_sec = seconds ? (dt.tm_sec / seconds) * seconds : 0,
                .tm_min = minutes ? (dt.tm_min / minutes) * minutes : 0,
                .tm_hour = hours ? (dt.tm_hour / hours) * hours : 0,
                .tm_mday = days ? ((dt.tm_mday - 1) / days) * days + 1 : 1,
                .tm_mon = months ? (dt.tm_mon / months) * months : 0,
                .tm_year = years ? ((dt.tm_year + TM_YEAR_OFFSET) / years) * years - TM_YEAR_OFFSET : 0,
                .tm_wday = 0,
                .tm_yday = 0,
                .tm_isdst = -1,
            };

            return to_timepoint(mktime(aligned, local), 0);
        }

        //==========================================================================
        // Public functions

        void tp_to_stream(std::ostream &stream,
                          const TimePoint &tp,
                          bool local,
                          uint decimals,
                          const std::string &format)
        {
            timespec ts = dt::to_timespec(tp);
            std::tm dt = local ? dt::localtime(ts.tv_sec) : dt::gmtime(ts.tv_sec);
            stream << std::put_time(&dt, format.c_str());

            /// Produce fractional seconds (e.g. decimals==3 -> milliseconds)
            if (decimals > 0)
            {
                long count = ts.tv_nsec;
                for (uint d = 9; d > decimals; d--)
                {
                    count /= 10;
                }

                stream << "." << std::fixed << std::setw(std::min(decimals, 9U))
                       << std::setfill('0') << count;
            }
        }

        void dur_to_stream(std::ostream &stream,
                           const Duration &dur,
                           uint decimals,
                           const std::string &format)
        {
            if (dur < dt::Duration::zero())
            {
                stream << "-";
            }
            tp_to_stream(stream, TimePoint(std::chrono::abs(dur)), false, decimals, format);
        }

        void dur_to_stream(std::ostream &stream,
                           const Duration &dur,
                           const std::optional<std::string> &secondsformat,
                           const std::optional<std::string> &minutesformat,
                           const std::optional<std::string> &hoursformat,
                           const std::optional<std::string> &daysformat,
                           const std::optional<std::string> &monthsformat,
                           const std::optional<std::string> &yearsformat,
                           uint maxdivs,
                           const std::string delimiter)
        {
            if (dur < dt::Duration::zero())
            {
                stream << "-";
            }
            double d = dt::to_double(std::chrono::abs(dur));
            std::string sep;
            uint divs = 0;

            if (d >= LEAP)
            {
                d -= std::trunc(d / LEAP) * DAY;
            }

            if (d >= YEAR && yearsformat.has_value())
            {
                sep = delimiter;
                uint years = static_cast<uint>(std::trunc(d / YEAR));
                str::format(stream, yearsformat.value(), years);
                d -= years * YEAR;
                divs++;
            }
            else if (divs > 0)
            {
                divs++;
            }

            if (d >= MONTH && monthsformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint months = static_cast<uint>(std::trunc(d / MONTH));
                str::format(stream, monthsformat.value(), months);
                d -= months * MONTH;
                divs++;
            }
            else if (divs > 0)
            {
                divs++;
            }

            if (d >= DAY && daysformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint days = static_cast<uint>(std::trunc(d / DAY));
                str::format(stream, daysformat.value(), days);
                d -= days * DAY;
                divs++;
            }
            else if (divs > 0)
            {
                divs++;
            }

            if (d >= HOUR && hoursformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint hours = static_cast<uint>(std::trunc(d / HOUR));
                str::format(stream, hoursformat.value(), hours);
                d -= hours * HOUR;
                divs++;
            }
            else if (divs > 0)
            {
                divs++;
            }

            if (d >= MINUTE && minutesformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint minutes = static_cast<uint>(std::trunc(d / MINUTE));
                str::format(stream, minutesformat.value(), minutes);
                d -= minutes * MINUTE;
                divs++;
            }
            else if (divs > 0)
            {
                divs++;
            }

            if ((d || sep.empty()) && secondsformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                str::format(stream, secondsformat.value(), d);
            }
        }

        std::string to_js_string(const TimePoint &tp)
        {
            return to_string(tp, false, 3, JS_FORMAT) + "Z";
        }

        std::string to_string(const TimePoint &tp,
                              bool local,
                              uint decimals,
                              const std::string &format)
        {
            std::ostringstream stream;
            tp_to_stream(stream, tp, local, decimals, format);
            return stream.str();
        }

        std::string to_string(const TimePoint &tp,
                              uint decimals,
                              const std::string &format)
        {
            return dt::to_string(tp, true, decimals, format);
        }

        std::string to_string(const Duration &duration,
                              uint decimals,
                              const std::string &format)
        {
            std::ostringstream stream;
            dur_to_stream(stream, duration, decimals, format);
            return stream.str();
        }

        std::string to_string(const Duration &duration,
                              const std::optional<std::string> &secondsformat,
                              const std::optional<std::string> &minutesformat,
                              const std::optional<std::string> &hoursformat,
                              const std::optional<std::string> &daysformat,
                              const std::optional<std::string> &monthsformat,
                              const std::optional<std::string> &yearsformat,
                              uint max_divisions,
                              const std::string &delimiter)
        {
            std::ostringstream stream;
            dur_to_stream(stream,
                          duration,
                          secondsformat,
                          minutesformat,
                          hoursformat,
                          daysformat,
                          monthsformat,
                          yearsformat,
                          max_divisions,
                          delimiter);
            return stream.str();
        }

        std::string to_string(const std::tm &tm,
                              const std::string &format)
        {
            std::ostringstream stream;
            stream << std::put_time(&tm, format.c_str());
            return stream.str();
        }

        timespec to_timespec(const TimePoint &tp)
        {
            auto seconds = std::chrono::floor<std::chrono::seconds>(tp);
            auto nanos = std::chrono::time_point_cast<std::chrono::nanoseconds>(tp) -
                         std::chrono::time_point_cast<std::chrono::nanoseconds>(seconds);

            struct timespec ts;
            // ts.tv_sec = dt::to_time_t(seconds);

            ts.tv_sec = seconds.time_since_epoch().count();
            ts.tv_nsec = (long)nanos.count();
            return ts;
        }

        std::time_t to_time_t(const TimePoint &tp)
        {
            return Clock::to_time_t(tp);
            // return dt::to_timespec(tp).tv_sec;
        }

        std::int64_t to_seconds(const Duration &d)
        {
            return std::chrono::floor<std::chrono::seconds>(d).count();
        }

        std::int64_t to_milliseconds(const TimePoint &tp)
        {
            return to_milliseconds(tp.time_since_epoch());
        }

        std::int64_t to_milliseconds(const Duration &d)
        {
            return std::chrono::floor<std::chrono::milliseconds>(d).count();
        }

        double to_double(const TimePoint &tp)
        {
            return dt::to_double(tp.time_since_epoch());
        }

        double to_double(const Duration &d)
        {
            return std::chrono::duration<double>(d).count();
        }

        //--------------------------------------------------------------------------
        // Duration conversions

        Duration ms_to_duration(std::int64_t milliseconds)
        {
            return std::chrono::duration_cast<Duration>(
                std::chrono::milliseconds(milliseconds));
        }

        Duration to_duration(time_t seconds, long nanoseconds)
        {
            return std::chrono::duration_cast<Duration>(
                std::chrono::seconds(seconds) + std::chrono::nanoseconds(nanoseconds));
        }

        Duration to_duration(double scalar, double multiplier)
        {
            return to_duration(scalar * multiplier);
        }

        Duration to_duration(double seconds)
        {
            return to_duration(static_cast<std::time_t>(seconds),
                               static_cast<long>((seconds - (int)seconds) * 1e9));
        }

        Duration to_duration(const timespec &ts)
        {
            return to_duration(ts.tv_sec, ts.tv_nsec);
        }

        Duration to_duration(const std::string_view &input,
                             const std::optional<std::string> &format,
                             const Duration fallback)
        {
            return format
                     ? try_to_duration(input, format.value()).value_or(dt::Duration::zero())
                     : try_to_duration(input).value_or(dt::Duration::zero());
        }

        std::optional<Duration> try_to_duration(const std::string_view &input)
        {
            static const std::regex rx(
                "(\\d{2}):(\\d{2}):(\\d{2})(\\.\\d+)?");

            std::match_results<std::string_view::iterator> match;
            if (std::regex_match(input.begin(), input.end(), match, rx))
            {
                double seconds =
                    (str::convert_to<std::uint32_t>(match.str(1)) * 3600) +
                    (str::convert_to<std::uint32_t>(match.str(2)) * 60) +
                    (str::convert_to<std::uint32_t>(match.str(3))) +
                    (match.length(4)
                         ? str::convert_to<double>(match.str(4))
                         : 0.0);
                return to_duration(seconds);
            }

            else if (auto opt_seconds = str::try_convert_to<double>(input))
            {
                return to_duration(opt_seconds.value());
            }
            else
            {
                return {};
            }
        }

        std::optional<Duration> try_to_duration(const std::string_view &input,
                                                const std::string &format)
        {
            if (auto tp = try_to_timepoint(input, format, false))
            {
                return tp->time_since_epoch();
            }
            else
            {
                return {};
            }
        }

        //--------------------------------------------------------------------------
        // Timepoint conversions

        TimePoint double_to_timepoint(
            double scalar,
            const std::optional<int> &multiplier_decimal_exponent)
        {
            if (multiplier_decimal_exponent)
            {
                scalar *= pow(10.0, 9 + *multiplier_decimal_exponent);
            }
            else
            {
                while ((scalar > 0) && (scalar < EPOCH_NANOS_LOWER_LIMIT))
                {
                    scalar *= 1000;
                }
            }
            return TimePoint(std::chrono::nanoseconds(static_cast<std::int64_t>(scalar)));
        }

        TimePoint int_to_timepoint(
            std::int64_t scalar,
            int multiplier_decimal_exponent)
        {
            while (--multiplier_decimal_exponent >= -9)
            {
                scalar *= 10;
            }
            return TimePoint(std::chrono::nanoseconds(scalar));
        }

        TimePoint int_to_timepoint(
            std::int64_t scalar)
        {
            while ((scalar > 0) && (scalar < EPOCH_NANOS_LOWER_LIMIT))
            {
                scalar *= 1000;
            }
            return TimePoint(std::chrono::nanoseconds(scalar));
        }

        TimePoint int_to_timepoint(
            std::int64_t scalar,
            const std::optional<int> &multiplier_decimal_exponent)
        {
            if (multiplier_decimal_exponent)
            {
                return int_to_timepoint(scalar, *multiplier_decimal_exponent);
            }
            else
            {
                return int_to_timepoint(scalar);
            }
        }

        TimePoint ms_to_timepoint(std::int64_t milliseconds)
        {
            return TimePoint(ms_to_duration(milliseconds));
        }

        TimePoint to_timepoint(time_t seconds, long nanoseconds)
        {
            return TimePoint(to_duration(seconds, nanoseconds));
        }

        TimePoint to_timepoint(const timespec &ts)
        {
            return to_timepoint(ts.tv_sec, ts.tv_nsec);
        }

        TimePoint to_timepoint(const std::tm &dt, bool local)
        {
            return to_timepoint(dt::mktime(dt, local), 0);
        }

        TimePoint to_timepoint(std::int32_t year,
                               std::uint32_t month,
                               std::uint32_t day,
                               std::uint32_t hour,
                               std::uint32_t minute,
                               std::uint32_t second,
                               double fraction,
                               std::optional<Duration> tz_offset)
        {
            std::tm tm_struct = {
                .tm_sec = static_cast<int>(second),
                .tm_min = static_cast<int>(minute),
                .tm_hour = static_cast<int>(hour),
                .tm_mday = static_cast<int>(day) - TM_DAY_OFFSET,
                .tm_mon = static_cast<int>(month) - TM_MONTH_OFFSET,
                .tm_year = static_cast<int>(year) - TM_YEAR_OFFSET,
                .tm_isdst = -1,
            };
            bool local = !tz_offset.has_value();

            std::time_t seconds = dt::mktime(tm_struct, local);
            std::int64_t nanos = (seconds + fraction) * 1e9;
            TimePoint tp{std::chrono::nanoseconds(nanos)};

            if (!local)
            {
                tp += tz_offset.value();
            }
            return tp;
        }

        TimePoint to_timepoint(
            const std::string_view &input,
            bool assume_local,
            const TimePoint &fallback,
            const std::optional<int> multiplier_decimal_exponent)
        {
            return try_to_timepoint(
                       input,
                       assume_local,
                       multiplier_decimal_exponent)
                .value_or(fallback);
        }

        TimePoint to_timepoint(
            const std::string_view &input,
            const std::string &format,
            bool assume_local,
            const TimePoint &fallback)
        {
            return try_to_timepoint(input, format, assume_local).value_or(fallback);
        }

        std::optional<TimePoint> try_to_timepoint(
            const std::string_view &input,
            bool assume_local,
            const std::optional<int> multiplier_decimal_exponent)
        {
            static const std::regex rx(
                "(\\d{4})-(\\d{2})-(\\d{2}).(\\d{2}):(\\d{2}):(\\d{2})(\\.\\d+)?(Z)?");

            std::match_results<std::string_view::iterator> match;
            if (std::regex_match(input.begin(), input.end(), match, rx))
            {
                std::optional<Duration> opt_zone_offset;
                if ((match.length(8) > 0) || !assume_local)
                {
                    // Time is provided in UTC -> zero offset.
                    opt_zone_offset = Duration::zero();
                }

                return core::dt::to_timepoint(
                    str::convert_to<std::int32_t>(match.str(1)),   // year
                    str::convert_to<std::uint32_t>(match.str(2)),  // month
                    str::convert_to<std::uint32_t>(match.str(3)),  // day
                    str::convert_to<std::uint32_t>(match.str(4)),  // hour
                    str::convert_to<std::uint32_t>(match.str(5)),  // minute
                    str::convert_to<std::uint32_t>(match.str(6)),  // second
                    match.length(7)                                // |-
                        ? str::convert_to<double>(match.str(7))    // |> fraction
                        : 0.0,                                     // |-
                    opt_zone_offset);                              // tz_offset
            }

            else if (auto opt_scalar = str::try_convert_to<std::uint64_t>(input))
            {
                return int_to_timepoint(opt_scalar.value(), multiplier_decimal_exponent);
            }
            else if (auto opt_scalar = str::try_convert_to<double>(input))
            {
                return double_to_timepoint(opt_scalar.value(), multiplier_decimal_exponent);
            }
            else
            {
                return {};
            }
        }

        std::optional<TimePoint> try_to_timepoint(
            const std::string_view &input,
            const std::string &format,
            bool assume_local)
        {
            std::tm dt = {};
            if (const char *end = ::strptime(input.data(), format.c_str(), &dt))
            {
                return to_timepoint(dt, (*end != 'Z') && assume_local);
            }
            else
            {
                return {};
            }
        }

        //--------------------------------------------------------------------------
        // Zone alignment and conversions

        TimePoint last_midnight(const TimePoint &tp, bool local)
        {
            return last_aligned(tp, std::chrono::hours(24), local);
        }

        TimePoint last_aligned(const TimePoint &tp,
                               const DateTimeInterval &interval,
                               bool local)
        {
            using namespace std::chrono;
            switch (interval.unit)
            {
            case TimeUnit::NANOSECOND:
                return last_aligned_tp<nanoseconds, seconds>(tp, interval.count);

            case TimeUnit::MICROSECOND:
                return last_aligned_tp<microseconds, seconds>(tp, interval.count);

            case TimeUnit::MILLISECOND:
                return last_aligned_tp<milliseconds, seconds>(tp, interval.count);

            case TimeUnit::SECOND:
                return last_aligned_tp<seconds, minutes>(tp, interval.count);

            case TimeUnit::MINUTE:
                return last_aligned(tp, std::chrono::minutes(interval.count), local);

            case TimeUnit::HOUR:
                return last_aligned(tp, std::chrono::hours(interval.count), local);

            case TimeUnit::DAY:
                return last_aligned_dt(tp, 1, 1, interval.count, 0, 0, 0, local);

            case TimeUnit::WEEK:
                // Use midnight, last Sunday (weekday 0) as reference.
                return last_aligned(
                    tp,
                    last_midnight(tp, local) - (localtime(tp).tm_wday * 24h),
                    interval.count * 7 * 24h);

            case TimeUnit::MONTH:
                return last_aligned_dt(tp, 1, interval.count, 0, 0, 0, 0, local);

            case TimeUnit::YEAR:
                return last_aligned_dt(tp, interval.count, 0, 0, 0, 0, 0, local);

            case TimeUnit::ETERNITY:
                return {};

            default:
                break;
            }

            return tp;
        }

        TimePoint last_aligned(const TimePoint &tp,
                               const Duration &interval,
                               bool local)
        {
            TimePoint reference = dt::epoch;
            if (local)
            {
                reference -= local_adjustment(tp);
            }
            return last_aligned(tp,
                                reference,
                                std::chrono::floor<std::chrono::seconds>(interval));
        }

        TimePoint last_aligned(const TimePoint &tp,
                               const TimePoint &reference,
                               const Duration &interval)
        {
            Duration offset = (tp - reference) % interval;
            if (offset < Duration::zero())
            {
                offset += interval;
            }
            return tp - offset;
        }

        Duration local_adjustment(const TimePoint &tp)
        {
            return tzinfo(tp).offset;
        }

        Duration local_adjustment(const TimePoint &tp,
                                  const std::string &timezone)
        {
            return tzinfo(timezone, tp).offset;
        }

        std::tm gmtime(const TimePoint &tp)
        {
            return dt::gmtime(dt::to_time_t(tp));
        }

        std::tm gmtime(const std::time_t &time)
        {
            return platform::timezone->gmtime(time);
        }

        std::tm localtime(const TimePoint &tp, const std::string &timezone)
        {
            return dt::localtime(dt::to_time_t(tp), timezone);
        }

        std::tm localtime(const TimePoint &tp)
        {
            return dt::localtime(dt::to_time_t(tp));
        }

        std::tm localtime(const std::time_t &time, const std::string &timezone)
        {
            return platform::timezone->localtime(time, timezone);
        }

        std::tm localtime(const std::time_t &time)
        {
            return platform::timezone->localtime(time);
        }

        TimeZoneInfo tzinfo(const std::string &timezone, const TimePoint &tp)
        {
            return dt::tzinfo(timezone, dt::to_time_t(tp));
        }

        TimeZoneInfo tzinfo(const TimePoint &tp)
        {
            return dt::tzinfo(dt::to_time_t(tp));
        }

        TimeZoneInfo tzinfo(const std::string &timezone, const std::time_t &time)
        {
            return platform::timezone->tzinfo(timezone, time);
        }

        TimeZoneInfo tzinfo(const std::time_t &time)
        {
            return platform::timezone->tzinfo(time);
        }

        std::time_t mktime(std::tm dt, bool local)
        {
            if (local)
            {
                return std::mktime(&dt);
            }
            else
            {
                // There is no `std::mktime()` equivalent for UTC, so we calculate this on our own.
                static uint gregorian_days_to_epoch = gregorian_days(dt::gmtime(epoch));
                int days = gregorian_days(dt) - gregorian_days_to_epoch;
                return ((days * 24 + dt.tm_hour) * 60 + dt.tm_min) * 60 + dt.tm_sec;
            }
        }

        std::uint32_t gregorian_days(const std::tm &dt)
        {
            uint month = (dt.tm_mon % 12) + TM_MONTH_OFFSET;
            uint year = (dt.tm_year + TM_YEAR_OFFSET) + (dt.tm_mon / 12);
            uint day = dt.tm_mday + TM_DAY_OFFSET;
            uint leaps = ((year - 1) / 4) - ((year - 1) / 100) + ((year - 1) / 400);
            return (365 * year) + leaps + day_of_year(year, month, day);
        }

        bool is_leap_year(uint year, bool gregorian)
        {
            bool leap = (year % 4 == 0);
            if (gregorian)
            {
                leap &= year % 100 != 0;
                leap |= year % 400 == 0;
            }
            return leap;
        }

        std::uint32_t day_of_year(uint year, uint month, uint day, bool gregorian)
        {
            static const uint mdays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            uint months = std::clamp(month - 1, 0u, 11u);
            uint days = day;

            for (uint m = 0; m < months; m++)
            {
                days += mdays[m];
            }

            if (month > 2)
            {
                days += is_leap_year(year, gregorian);
            }

            return days;
        }

    }  // namespace dt

    namespace steady
    {
        const TimePoint epoch;

        TimePoint to_timepoint(dt::TimePoint tp)
        {
            return Clock::now() - dt::Clock::now().time_since_epoch() + tp.time_since_epoch();
        }

        void tp_to_stream(std::ostream &stream,
                          const TimePoint &stp,
                          uint decimals)
        {
            dt::dur_to_stream(stream,
                              std::chrono::duration_cast<dt::Duration>(stp.time_since_epoch()),
                              decimals);
        }
    }  // namespace steady
}  // namespace core

namespace std::chrono
{
    std::ostream &operator<<(std::ostream &stream, const core::dt::TimePoint &tp)
    {
        core::dt::tp_to_stream(stream, tp);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const core::dt::Duration &dur)
    {
        core::dt::dur_to_stream(stream, dur);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const core::steady::TimePoint &stp)
    {
        core::steady::tp_to_stream(stream, stp);
        return stream;
    }

}  // namespace std::chrono

bool operator==(const std::tm &lhs, const std::tm &rhs)
{
    return ((lhs.tm_sec == rhs.tm_sec) &&
            (lhs.tm_min == rhs.tm_min) &&
            (lhs.tm_hour == rhs.tm_hour) &&
            (lhs.tm_mday == rhs.tm_mday) &&
            (lhs.tm_mon == rhs.tm_mon) &&
            (lhs.tm_year == rhs.tm_year) &&
            (lhs.tm_wday == rhs.tm_wday) &&
            (lhs.tm_yday == rhs.tm_yday) &&
            (lhs.tm_isdst == rhs.tm_isdst));
}

std::ostream &operator<<(std::ostream &stream, const std::tm &tm)
{
    stream << std::put_time(&tm, core::dt::DEFAULT_FORMAT);
    return stream;
}
