// -*- c++ -*-
//==============================================================================
/// @file sysconfig-timezone-api.h++
/// @brief SysConfig service - Time Zone configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/provider.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace platform::sysconfig
{
    //======================================================================
    // TimeZoneSpec

    struct TimeZoneSpec
    {
        std::string zonename;        // TimeZone name, e.g. America/Los_Angeles
        std::string continent;       // World region, e.g. America
        std::string countrycode;     // 2-letter country code, e.g. US
        std::string countryname;     // Fully country name, e.g. United States
        std::string displayname;     // Display name within region, e.g. Pacific
        std::int32_t latitude = 0;   // Latitude, seconds north
        std::int32_t longitude = 0;  // Longitude, seconds east
    };

    std::ostream &operator<<(std::ostream &stream, const TimeZoneSpec &spec);

    //======================================================================
    // TimeZoneConfig

    struct TimeZoneConfig
    {
        std::string zonename;  // Configured zone, e.g. America/Los_Angeles
        bool automatic;        // Whether timezone is automatically detected
        std::string provider;  // Timezone service, if any.
    };

    std::ostream &operator<<(std::ostream &stream, const TimeZoneConfig &spec);

    //==========================================================================
    // Type aliases

    using TimeZoneList = std::vector<TimeZoneSpec>;
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

        /// Obtain information about available time zones
        virtual std::vector<TimeZoneSpec> get_timezone_specs() const = 0;

        /// Return geographic information about an arbitrary timezone.
        /// If no zone name is provided, return information about the configured zone.
        virtual TimeZoneSpec get_timezone_spec(const std::string &zone = {}) const = 0;

        /// Get or set the timezone configuration
        virtual TimeZoneInfo set_timezone(const TimeZoneConfig &config) = 0;
        virtual TimeZoneConfig get_configured_timezone() const = 0;
        virtual TimeZoneInfo get_current_timezone() const = 0;
    };

    //==========================================================================
    // Provider proxy

    extern core::platform::ProviderProxy<TimeZoneInterface> timezone;

    //==========================================================================
    // Signals

    extern core::signal::DataSignal<TimeZoneInfo> signal_tzinfo;
    extern core::signal::DataSignal<TimeZoneConfig> signal_tzconfig;

}  // namespace platform::sysconfig
