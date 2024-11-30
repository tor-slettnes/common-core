// -*- c++ -*-
//==============================================================================
/// @file sysconfig-timezone.c++
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
        core::types::TaggedValueList tvlist;
        tvlist.append_if(!country.code.empty(), "code", country.code);
        tvlist.append_if(!country.name.empty(), "name", country.name);
        tvlist.to_stream(stream);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocation &location)
    {
        core::types::TaggedValueList tvlist;
        tvlist.emplace_back("country", core::str::convert_from(location.country));
        tvlist.append_if(!location.region.empty(), "region", location.region);
        tvlist.to_stream(stream);
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocationFilter &filter)
    {
        core::types::TaggedValueList tvlist;

        tvlist.append_if(!filter.area.empty(),
                          "area",
                          filter.area);

        tvlist.append_if(!filter.country.code.empty() || !filter.country.name.empty(),
                          "country",
                          core::str::convert_from(filter.country));

        tvlist.to_stream(stream);
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
