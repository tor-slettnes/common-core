/// -*- c++ -*-
//==============================================================================
/// @file windows-timezone.c++
/// @brief Timezone functions - WINDOWS implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-timezone.h++"
#include <sstream>

namespace shared::platform
{
    WindowsTimeZoneProvider::WindowsTimeZoneProvider()
        : Super("WindowsTimeZoneProvider")
    {
    }

    std::tm WindowsTimeZoneProvider::gmtime(const std::time_t &time) const
    {
        std::tm dt;
        gmtime_s(&dt, &time);
        return dt;
    }

    std::tm WindowsTimeZoneProvider::localtime(const std::time_t &time, const std::string &timezone) const
    {
        // TODO: Support `timezone` argument
        return localtime(time);
    }

    std::tm WindowsTimeZoneProvider::localtime(const time_t &time) const
    {
        // SYSTEMTIME utctime = systemtime_encode(gmtime(time));
        // SYSTEMTIME localtime;
        // SystemTimeToTzSpecificLocalTime(nullptr, &utctime, &localtime);
        // return systemtime_decode(localtime);

        std::tm dt;
        localtime_s(&dt, &time);
        return dt;
    }

    dt::TimeZoneInfo WindowsTimeZoneProvider::tzinfo(const std::string &timezone, const std::time_t &time) const
    {
        // TODO: Support `timezone` argument
        return tzinfo(time);
    }

    dt::TimeZoneInfo WindowsTimeZoneProvider::tzinfo(const std::time_t &time) const
    {
        TIME_ZONE_INFORMATION info;
        uint dstflag = GetTimeZoneInformation(&info);
        bool dst = (dstflag == TIME_ZONE_ID_DAYLIGHT);
        dt::Duration stdoffset = std::chrono::minutes(-info.Bias);
        dt::Duration dstoffset = std::chrono::minutes(-info.Bias - info.DaylightBias);
        std::string zonename = str::from_wstring(dst ? info.DaylightName : info.StandardName);

        return {
            this->filter_uppercase(zonename),  // shortname
            dst ? dstoffset : stdoffset,       // offset
            stdoffset,                         // stdoffset
            dst                                // dst
        };
    }

    std::string WindowsTimeZoneProvider::filter_uppercase(const std::string &input) const
    {
        std::stringstream result;
        for (unsigned char c : input)
        {
            if (std::isupper(c))
            {
                result << c;
            }
        }
        return result.str();
    }

    std::string WindowsTimeZoneProvider::utf8encode(const std::wstring &wstr) const
    {
        if (wstr.empty())
        {
            return std::string();
        }

        int size_needed = WideCharToMultiByte(
            CP_UTF8,           // codepage
            0,                 // dwFlags
            wstr.data(),       // lpWiderCharStr
            (int)wstr.size(),  // sharedWideChar
            nullptr,           // lpMultiByteStr
            0,                 // cbMultiByte
            nullptr,           // lpDefaultChar
            nullptr);          // lpUsedDefaultChar

        std::string target(size_needed, 0);

        WideCharToMultiByte(
            CP_UTF8,           // codepage
            0,                 // dwFlags
            wstr.data(),       // lpWiderCharStr
            (int)wstr.size(),  // sharedWideChar
            target.data(),     // lpMultiByteStr
            size_needed,       // cbMultiByte
            nullptr,           // lpDefaultChar
            nullptr);          // lpUsedDefaultChar

        return target;
    }

    std::tm WindowsTimeZoneProvider::systemtime_decode(const SYSTEMTIME &st) const
    {
        std::tm dt;
        dt.tm_sec = st.wSecond;
        dt.tm_min = st.wMinute;
        dt.tm_hour = st.wHour;
        dt.tm_mday = st.wDay - dt::TM_DAY_OFFSET;
        dt.tm_mon = st.wMonth - dt::TM_MONTH_OFFSET;
        dt.tm_year = st.wYear - dt::TM_YEAR_OFFSET;
        dt.tm_wday = (st.wDayOfWeek - dt::TM_WEEKDAY_OFFSET) % 7;
        dt.tm_yday = dt::day_of_year(st.wYear, st.wMonth, st.wDay);
        dt.tm_isdst = 0;  // TODO: Determine
        return dt;
    }

    SYSTEMTIME WindowsTimeZoneProvider::systemtime_encode(const std::tm &dt) const
    {
        SYSTEMTIME st;
        st.wYear = dt.tm_year + dt::TM_YEAR_OFFSET;
        st.wMonth = dt.tm_mon + dt::TM_MONTH_OFFSET;
        st.wDayOfWeek = (dt.tm_wday + dt::TM_WEEKDAY_OFFSET) % 7;
        st.wDay = dt.tm_mday + dt::TM_DAY_OFFSET;
        st.wHour = dt.tm_hour;
        st.wMinute = dt.tm_min;
        st.wSecond = dt.tm_sec;
        st.wMilliseconds = 0;
        return st;
    }

}  // namespace shared::platform
