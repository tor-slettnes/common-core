// -*- c++ -*-
//==============================================================================
/// @file system-timezone-api.h++
/// @brief System service - Time Zone configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-types.h++"
#include "platform/provider.h++"
#include "chrono/date-time.h++"
#include "thread/signaltemplate.h++"

namespace platform::system
{
    class TimeZone : public core::platform::Provider
    {
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
        virtual core::dt::TimeZoneInfo get_current_timezone() const = 0;
    };

    extern core::platform::ProviderProxy<TimeZone> timezone;

    extern core::signal::DataSignal<core::dt::TimeZoneInfo> signal_tzinfo;
    extern core::signal::DataSignal<TimeZoneConfig> signal_tzconfig;

}  // namespace platform::system
