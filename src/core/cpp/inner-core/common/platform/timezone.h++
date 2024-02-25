/// -*- c++ -*-
//==============================================================================
/// @file timezone.h++
/// @brief Timezone functions - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "chrono/date-time.h++"

namespace shared::platform
{
    class TimeZoneProvider : public Provider
    {
        using This = TimeZoneProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        virtual std::tm gmtime(const std::time_t &time) const = 0;
        virtual std::tm localtime(const std::time_t &time, const std::string &timezone) const = 0;
        virtual std::tm localtime(const time_t &time) const = 0;
        virtual dt::TimeZoneInfo tzinfo(const std::string &timezone, const std::time_t &time) const = 0;
        virtual dt::TimeZoneInfo tzinfo(const std::time_t &time) const = 0;

        // virtual std::string get_configured_zone() const = 0;
        // virtual void set_configured_zone() = 0;
    };

    extern ProviderProxy<TimeZoneProvider> timezone;
}  // namespace shared::platform
