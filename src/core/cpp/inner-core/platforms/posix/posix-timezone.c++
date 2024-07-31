/// -*- c++ -*-
//==============================================================================
/// @file posix-timezone.c++
/// @brief Timezone functions - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-timezone.h++"
#include "platform/runtime.h++"
#include "logging/logging.h++"

#include <mutex>
#include <ctime>

#define TZENV "TZ"

namespace core::platform
{
    PosixTimeZoneProvider::PosixTimeZoneProvider()
        : Super("PosixTimeZoneProvider")
    {
    }

    std::tm PosixTimeZoneProvider::gmtime(const std::time_t &time) const
    {
        std::tm dt;
        gmtime_r(&time, &dt);
        return dt;
    }

    std::tm PosixTimeZoneProvider::localtime(const std::time_t &time, const std::string &timezone) const
    {
        auto lck = std::lock_guard(const_cast<This *>(this)->mtx);
        SavedValue saved = this->apply_zone(timezone);
        std::tm dt = this->localtime(time);
        this->restore_zone(saved);
        return dt;
    }

    std::tm PosixTimeZoneProvider::localtime(const time_t &time) const
    {
        std::tm dt;
        localtime_r(&time, &dt);
        return dt;
    }

    dt::TimeZoneInfo PosixTimeZoneProvider::tzinfo(
        const std::string &timezone,
        const std::time_t &time) const
    {
        auto lck = std::lock_guard(const_cast<This *>(this)->mtx);
        SavedValue saved = this->apply_zone(timezone);
        dt::TimeZoneInfo zi = this->tzinfo(time);
        this->restore_zone(saved);
        return zi;
    }

    dt::TimeZoneInfo PosixTimeZoneProvider::tzinfo(const std::time_t &time) const
    {
        tzset();

        std::tm local_tm;
        localtime_r(&time, &local_tm);

        std::time_t local_time = dt::mktime(local_tm, false);
        dt::Duration offset = std::chrono::seconds(local_time - time);
        dt::Duration stdoffset = offset - std::chrono::hours(local_tm.tm_isdst);

        return {
            tzname[local_tm.tm_isdst > 0],  // Effective zone, e.g., "PST" or "PDT"
            offset,                         // Current timezone offset, east of UTC
            stdoffset,                      // Standard timezone offset, east of UTC
            local_tm.tm_isdst > 0,          // Daylight savings time flag
        };
    }

    PosixTimeZoneProvider::SavedValue PosixTimeZoneProvider::apply_zone(
        const std::string &zonename) const
    {
        std::optional<std::string> tzrestore = platform::runtime->getenv(TZENV);

        // Change zone
        platform::runtime->setenv(TZENV, zonename);
        tzset();
        return tzrestore;
    }

    void PosixTimeZoneProvider::restore_zone(const SavedValue &saved) const
    {
        // Restore zone
        if (saved)
        {
            platform::runtime->setenv(TZENV, saved.value());
        }
        else
        {
            platform::runtime->unsetenv(TZENV);
        }
        tzset();
    }

}  // namespace core::platform
