// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-timezone.h++
/// @brief SysConfig service - Time Zone Configuration POSIX Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-timezone.h++"
#include "types/valuemap.h++"

#define READCHUNK 256
#define TZROOT    "/usr/share/zoneinfo"
#define TZLINK    "/etc/localtime"
#define TZFILE    "/etc/timezone"
#define CCFILE    TZROOT "/zone1970.tab"
#define CNFILE    TZROOT "/iso3166.tab"

namespace platform::sysconfig::native
{
    using CountryMap = core::types::ValueMap<std::string, std::string>;
    using TimeZoneMap = std::multimap<std::string, TimeZoneSpec>;

    class PosixTimeZoneProvider : public TimeZoneInterface
    {
        using This = PosixTimeZoneProvider;
        using Super = TimeZoneInterface;

    protected:
        PosixTimeZoneProvider();

    public:
        bool is_pertinent() override;
        void initialize() override;
        void deinitialize() override;

    protected:
        //==========================================================================
        // Time zone configuration

        /// Obtain information about available time zones
        std::vector<TimeZoneSpec> get_timezone_specs() const override;

        /// Return geographic information about an arbitrary timezone.
        /// If no zone name is provided, return information about the configured zone.
        TimeZoneSpec get_timezone_spec(const std::string &zonename = {}) const override;

        /// Get or set the timezone configuration
        TimeZoneInfo set_timezone(const TimeZoneConfig &config) override;
        TimeZoneConfig get_configured_timezone() const override;
        TimeZoneInfo get_current_timezone() const override;

    private:
        std::string get_configured_zonename() const;
        void set_configured_zonename(const std::string &zone);

        TimeZoneList load_zones(const fs::path &zonetab = CCFILE) const;
        TimeZoneMap load_zone_map(const fs::path &zonetab = CCFILE) const;

        CountryMap load_countries(const fs::path &cctab = CNFILE) const;
        int to_scalar_coord(const std::string &coord) const;
    };
}  // namespace platform::sysconfig::native
