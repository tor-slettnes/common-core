/// -*- c++ -*-
//==============================================================================
/// @file date-time.c++
/// @brief Time/clock utilities
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "date-time.h++"
#include "string/format.h++"
#include "platform/timezone.h++"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

namespace shared
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
                uint years = (uint)std::trunc(d / YEAR);
                str::format(stream, yearsformat.value(), years);
                d -= years * YEAR;
                divs++;
            }

            if ((d >= MONTH || divs > 0) && monthsformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint months = (uint)std::trunc(d / MONTH);
                str::format(stream, monthsformat.value(), months);
                d -= months * MONTH;
                divs++;
            }

            if ((d >= DAY || divs > 0) && daysformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint days = (uint)std::trunc(d / DAY);
                str::format(stream, daysformat.value(), days);
                d -= days * DAY;
                divs++;
            }

            if ((d >= HOUR || divs > 0) && hoursformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint hours = (uint)std::trunc(d / HOUR);
                str::format(stream, hoursformat.value(), hours);
                d -= hours * HOUR;
                divs++;
            }

            if ((d >= MINUTE || divs > 0) && minutesformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                sep = delimiter;
                uint minutes = (uint)std::trunc(d / MINUTE);
                str::format(stream, minutesformat.value(), minutes);
                d -= minutes * MINUTE;
                divs++;
            }

            if ((d || sep.empty()) && secondsformat.has_value() && divs < maxdivs)
            {
                stream << sep;
                str::format(stream, secondsformat.value(), d);
            }
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

        std::string to_string(const Duration &dur,
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
                          dur,
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
            auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);
            if (seconds > tp)
            {
                seconds -= std::chrono::seconds(1);
            }
            auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(tp - seconds);

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

        long long to_seconds(const Duration &d)
        {
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);
            if (seconds > d)
            {
                seconds -= std::chrono::seconds(1);
            }
            return seconds.count();
        }

        double to_double(const TimePoint &tp)
        {
            return dt::to_double(tp.time_since_epoch());
        }

        double to_double(const Duration &d)
        {
            return std::chrono::duration<double>(d).count();
        }

        Duration to_duration(double seconds)
        {
            return to_duration(static_cast<std::time_t>(seconds),
                               static_cast<long>((seconds - (int)seconds) * 1e9));
        }

        Duration to_duration(time_t seconds, long nanoseconds)
        {
            return std::chrono::duration_cast<Duration>(
                std::chrono::seconds(seconds) + std::chrono::nanoseconds(nanoseconds));
            // return std::chrono::seconds(seconds) + std::chrono::nanoseconds(nanoseconds);
        }

        Duration to_duration(const timespec &ts)
        {
            return to_duration(ts.tv_sec, ts.tv_nsec);
        }

        Duration to_duration(const std::string &string, const std::string &format)
        {
            return to_timepoint(string, false, format).time_since_epoch();
        }

        /// Convert from "struct tm" to TimePoint, or fallback if the time is zero.
        TimePoint to_timepoint(const std::tm &dt, bool local, const TimePoint &fallback)
        {
            return to_timepoint(dt::mktime(dt, local), 0, fallback);
        }

        TimePoint to_timepoint(const timespec &ts, const TimePoint &fallback)
        {
            return to_timepoint(ts.tv_sec, ts.tv_nsec, fallback);
        }

        TimePoint to_timepoint(double seconds, const TimePoint &fallback)
        {
            Duration duration = to_duration(seconds);
            if (duration != Clock::duration::zero())
            {
                return TimePoint(duration);
            }
            else
            {
                return fallback;
            }
        }

        TimePoint to_timepoint(time_t seconds, long nanoseconds, const TimePoint &fallback)
        {
            Duration duration = to_duration(seconds, nanoseconds);
            if (duration != Clock::duration::zero())
            {
                return TimePoint(duration);
            }
            else
            {
                return fallback;
            }
        }

        TimePoint to_timepoint(const std::string &string,
                               bool local,
                               const std::string &format,
                               const TimePoint &fallback)
        {
            std::tm dt = {};
            std::stringstream ss(string);
            ss >> std::get_time(&dt, format.c_str());
            if (ss.fail())
            {
                return fallback;
            }
            return to_timepoint(dt, local);
        }

        TimePoint to_timepoint(steady::TimePoint stp)
        {
            return Clock::now() -
                   std::chrono::duration_cast<Duration>(
                       steady::Clock::now().time_since_epoch() +
                       stp.time_since_epoch());
        }

        TimePoint last_midnight(const TimePoint &tp, bool local)
        {
            return last_aligned(tp, std::chrono::hours(24), local);
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
            return last_aligned(tp, reference, interval);
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
}  // namespace shared

namespace std::chrono
{
    std::ostream &operator<<(std::ostream &stream, const shared::dt::TimePoint &tp)
    {
        shared::dt::tp_to_stream(stream, tp);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const shared::dt::Duration &dur)
    {
        shared::dt::dur_to_stream(stream, dur);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const shared::steady::TimePoint &stp)
    {
        shared::steady::tp_to_stream(stream, stp);
        return stream;
    }

}  // namespace std::chrono
