/// -*- c++ -*-
//==============================================================================
/// @file posix-timezone.c++
/// @brief Timezone functions - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-timezone.h++"
#include "platform/runtime.h++"

#include <mutex>
#include <ctime>

#define TZENV "TZ"

namespace cc::platform
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
        std::string tzrestore = this->apply_zone(timezone);
        std::tm dt = this->localtime(time);
        this->restore_zone(tzrestore);
        return dt;
    }

    std::tm PosixTimeZoneProvider::localtime(const time_t &time) const
    {
        std::tm dt;
        localtime_r(&time, &dt);
        return dt;
    }

    dt::TimeZoneInfo PosixTimeZoneProvider::tzinfo(const std::string &timezone, const std::time_t &time) const
    {
        auto lck = std::lock_guard(const_cast<This *>(this)->mtx);
        std::string tzrestore = this->apply_zone(timezone);
        dt::TimeZoneInfo zi = this->tzinfo(time);
        this->restore_zone(tzrestore);
        return zi;
    }

    dt::TimeZoneInfo PosixTimeZoneProvider::tzinfo(const std::time_t &time) const
    {
        tzset();

        std::tm local;
        localtime_r(&time, &local);

        std::time_t local_t = dt::mktime(local, false);

        dt::Duration offset = std::chrono::seconds(local_t - time);
        dt::Duration stdoffset = offset + std::chrono::hours(local.tm_isdst);

        return {
            tzname[local.tm_isdst > 0],  // Effective zone, e.g., "PST" or "PDT"
            offset,                      // Current timezone offset, east of UTC
            stdoffset,                   // Standard timezone offset, east of UTC
            local.tm_isdst > 0,          // Daylight savings time flag
        };
    }

    std::string PosixTimeZoneProvider::apply_zone(const std::string &zonename) const
    {
        std::string tzrestore = TZENV;
        // Backup & modify TZ environment variable
        if (char *envzone = std::getenv(TZENV))
        {
            tzrestore = tzrestore + "=" + envzone;
        }

        // Change zone
        platform::runtime->putenv(TZENV "=" + zonename);
        tzset();
        return tzrestore;
    }

    void PosixTimeZoneProvider::restore_zone(const std::string &saved) const
    {
        // Restore zone
        platform::runtime->putenv(saved);
        tzset();
    }

}  // namespace cc::platform
