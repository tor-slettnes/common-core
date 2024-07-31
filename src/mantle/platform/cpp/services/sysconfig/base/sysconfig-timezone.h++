// -*- c++ -*-
//==============================================================================
/// @file sysconfig-timezone-api.h++
/// @brief SysConfig service - Time Zone configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace platform::sysconfig
{
    //======================================================================
    // Time Zone Area/Location aliases

    using TimeZoneCanonicalName = std::string;
    using TimeZoneArea = std::string;
    using TimeZoneAreas = std::vector<TimeZoneArea>;
    using TimeZoneCountryCode = std::string;
    using TimeZoneCountryName = std::string;
    using TimeZoneRegion = std::string;
    using TimeZoneRegions = std::vector<TimeZoneRegion>;

    //======================================================================
    // Time Zone Country

    struct TimeZoneCountry
    {
        TimeZoneCountryCode code;  // ISO 3166 code, e.g. "US" or "DE"
        TimeZoneCountryName name;  // English name, e.g. "United States" or "Germany"
    };

    bool operator<(const TimeZoneCountry &lhs, const TimeZoneCountry &rhs);
    std::ostream &operator<<(std::ostream &stream, const TimeZoneCountry &country);
    using TimeZoneCountries = std::vector<TimeZoneCountry>;

    //======================================================================
    // Time Zone location (country + possible region)

    struct TimeZoneLocation
    {
        TimeZoneCountry country;
        TimeZoneRegion region;
    };

    using TimeZoneLocations = std::vector<TimeZoneLocation>;
    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocation &location);

    //======================================================================
    // Time Zone Area + Country filter

    struct TimeZoneLocationFilter
    {
        TimeZoneArea area;
        TimeZoneCountry country;
    };
    std::ostream &operator<<(std::ostream &stream, const TimeZoneLocationFilter &filter);

    //======================================================================
    // TimeZoneCanonicalSpec

    struct TimeZoneCanonicalSpec
    {
        // Canonical name ("Olson" name), e.g. `America/Los_Angeles` or `Etc/UTC`.
        TimeZoneCanonicalName name;

        // The `Area` component of the name, e.g. `America`, `Atlantic`, `Etc`.
        TimeZoneArea area;

        // One or more countries and possibly region within a country where this
        // time zone is used
        TimeZoneLocations locations;

        // Geographical cordinates of the canonical location. For instance the
        // coordinates of `Europe/Berlin` is that of Berlin, not any other
        // countries where it is used (e.g. Norway, Sweden, Denmark)
        std::int32_t latitude = 0;   // Seconds north of Equator
        std::int32_t longitude = 0;  // Seconds east of Prime Meridian
    };

    std::ostream &operator<<(std::ostream &stream, const TimeZoneCanonicalSpec &spec);

    using TimeZoneCanonicalSpecs = std::vector<TimeZoneCanonicalSpec>;

    //==========================================================================
    // Time Zone Offset Info

    using TimeZoneInfo = core::dt::TimeZoneInfo;

    //==========================================================================
    // TimeZoneProvider

    class TimeZoneInterface : public core::platform::Provider
    {
        using This = TimeZoneInterface;
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //==========================================================================
        // Time zone configuration

        virtual TimeZoneAreas list_timezone_areas() const = 0;

        virtual TimeZoneCountries list_timezone_countries(
            const TimeZoneArea &area = {}) = 0;

        virtual TimeZoneRegions list_timezone_regions(
            const TimeZoneLocationFilter &filter = {}) = 0;

        /// Obtain information about available time zones
        virtual TimeZoneCanonicalSpecs list_timezone_specs(
            const TimeZoneLocationFilter &filter = {}) const = 0;

        /// Return geographic information about a specific canonical timezone. If no
        /// zone name is specified, return info about the currently configured zone.
        virtual TimeZoneCanonicalSpec get_timezone_spec(
            const TimeZoneCanonicalName &zone = {}) const = 0;

        /// Get information about the currently applied timezone, including time
        /// offsets from UTC and whether Daylight Savings / Summer Time is currently
        /// in effect.
        virtual TimeZoneInfo get_timezone_info(
            const TimeZoneCanonicalName &canonical_zone = {},
            const core::dt::TimePoint &timepoint = {}) const = 0;

        /// Set the system time zone to the specified canonical name.
        /// Return the resulting applied zone
        virtual TimeZoneInfo set_timezone(
            const TimeZoneCanonicalName &zonename) = 0;

        /// Set the system timezone based on country and region, if applicable.
        virtual TimeZoneInfo set_timezone(
            const TimeZoneLocation &location) = 0;
    };

    //==========================================================================
    // Provider proxy

    extern core::platform::ProviderProxy<TimeZoneInterface> timezone;

    //==========================================================================
    // Signals

    extern core::signal::DataSignal<TimeZoneInfo> signal_tzinfo;
    extern core::signal::DataSignal<TimeZoneCanonicalName> signal_tzconfig;

}  // namespace platform::sysconfig
