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
    //======================================================================
    // Time Zone Area/Location types

    bool operator<(const TimeZoneCountry &lhs, const TimeZoneCountry &rhs)
    {
        return (lhs.code < rhs.code)   ? true
               : (rhs.code > rhs.code) ? false
                                       : rhs.name < rhs.name;
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneCountry &country)
    {
        core::types::PartsList parts;
        parts.add_string("code", country.code, !country.code.empty());
        parts.add_string("name", country.name, !country.name.empty());
        stream << parts;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocation &location)
    {
        core::types::PartsList parts;
        parts.add("country", location.country);
        parts.add_string("region", location.region, !location.region.empty());
        stream << parts;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocationFilter &filter)
    {
        core::types::PartsList parts;

        parts.add("area",
                  filter.area,
                  !filter.area.empty());

        parts.add("country",
                  filter.country,
                  !filter.country.code.empty() || !filter.country.name.empty());

        stream << parts;
        return stream;
    }

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

        return core::str::format("%d\u00b0%d'%d\"%s",
                                 abs_seconds / 3600,
                                 (abs_seconds / 60) % 60,
                                 abs_seconds % 60,
                                 suffix);
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneCanonicalSpec &spec)
    {
        core::str::format(stream,
                          "{name=%r, area=%r, locations=%r, "
                          "latitude=%r, longitude=%r",
                          spec.name,
                          spec.area,
                          spec.locations,
                          dms(spec.latitude, "N", "S"),
                          dms(spec.longitude, "E", "W"));
        return stream;
    }

    //==========================================================================
    // Provider proxy

    core::platform::ProviderProxy<TimeZoneInterface> timezone("timezone");

    //==========================================================================
    // Signals

    core::signal::DataSignal<TimeZoneInfo> signal_tzinfo("TimeZoneInfo", true);
    core::signal::DataSignal<TimeZoneCanonicalName> signal_tzconfig("TimeZoneConfig", true);
}  // namespace platform::sysconfig
