/// -*- c++ -*-
//==============================================================================
/// @file windows-timezone.h++
/// @brief Timezone functions - WINDOWS implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/timezone.h++"

#include <windows.h>

namespace core::platform
{
    class WindowsTimeZoneProvider : public TimeZoneProvider
    {
        using This = WindowsTimeZoneProvider;
        using Super = TimeZoneProvider;

    protected:
        WindowsTimeZoneProvider();

    public:
        std::tm gmtime(const std::time_t &time) const override;

        std::tm localtime(const std::time_t &time, const std::string &timezone) const override;
        std::tm localtime(const time_t &time) const override;

        dt::TimeZoneInfo tzinfo(const std::string &timezone, const std::time_t &time) const override;
        dt::TimeZoneInfo tzinfo(const std::time_t &time) const override;

    private:
        std::string filter_uppercase(const std::string &input) const;
        std::string utf8encode(const std::wstring &wstr) const;
        std::tm systemtime_decode(const SYSTEMTIME &st) const;
        SYSTEMTIME systemtime_encode(const std::tm &dt) const;
    };
}  // namespace core::platform
