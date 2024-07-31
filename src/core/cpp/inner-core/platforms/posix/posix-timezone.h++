/// -*- c++ -*-
//==============================================================================
/// @file posix-timezone.h++
/// @brief Timezone functions - POSIX implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/timezone.h++"

#include <mutex>
#include <optional>

namespace core::platform
{
    class PosixTimeZoneProvider : public TimeZoneProvider
    {
        using This = PosixTimeZoneProvider;
        using Super = TimeZoneProvider;
        using SavedValue = std::optional<std::string>;

    protected:
        PosixTimeZoneProvider();

    public:
        std::tm gmtime(const std::time_t &time) const override;

        std::tm localtime(const std::time_t &time, const std::string &timezone) const override;
        std::tm localtime(const time_t &time) const override;

        dt::TimeZoneInfo tzinfo(const std::string &timezone, const std::time_t &time) const override;
        dt::TimeZoneInfo tzinfo(const std::time_t &time) const override;

    private:
        SavedValue apply_zone(const std::string &zonename) const;
        void restore_zone(const SavedValue &saved) const;

    private:
        std::recursive_mutex mtx;
    };
}  // namespace core::platform
