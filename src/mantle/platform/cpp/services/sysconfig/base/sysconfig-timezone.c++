// -*- c++ -*-
//==============================================================================
/// @file sysconfig-timezone-api.c++
/// @brief SysConfig service - Time Zone configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-timezone.h++"
#include "string/format.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // TimeZoneSpec

    std::string dms(std::int32_t seconds,
                    std::string positive_suffix,
                    std::string negative_suffix)
    {
        std::int32_t abs_seconds = std::abs(seconds);

        std::string suffix =
            seconds > 0   ? positive_suffix
            : seconds < 0 ? negative_suffix
                          : ""s;

        return core::str::format("%d\xb0 %d' %d\" %s",
                                 abs_seconds / 3600,
                                 (abs_seconds / 60) % 60,
                                 abs_seconds % 60,
                                 suffix);
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneSpec &spec)
    {
        core::str::format(stream,
                          "{zonename=%r, continent=%r, "
                          "countrycode=%r, countryname=%r, displayname=%r, "
                          "latitude=%r, longitude=%r",
                          spec.zonename,
                          spec.continent,
                          spec.countrycode,
                          spec.countryname,
                          spec.displayname,
                          dms(spec.latitude, "N", "S"),
                          dms(spec.longitude, "E", "W"));
        return stream;
    }

    //==========================================================================
    // TimeZoneConfig

    std::ostream &operator<<(std::ostream &stream, const TimeZoneConfig &config)
    {
        core::types::PartsList parts;
        parts.add_string("zonename", config.zonename);
        parts.add_value("automatic", config.automatic);
        parts.add_string("provider", config.provider, !config.provider.empty());
        stream << parts;
        return stream;
    }

    //==========================================================================
    // Provider proxy

    core::platform::ProviderProxy<TimeZoneInterface> timezone("timezone");

    //==========================================================================
    // Signals

    core::signal::DataSignal<TimeZoneInfo> signal_tzinfo("TimeZoneInfo", true);
    core::signal::DataSignal<TimeZoneConfig> signal_tzconfig("TimeZoneConfig", true);
}  // namespace platform::sysconfig
