// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-timezone.h++
/// @brief SysConfig service - Time Zone Configuration POSIX Implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sysconfig-timezone.h++"
#include "types/valuemap.h++"
#include "settings/settingsstore.h++"

#define READCHUNK 256
#define TZROOT    "/usr/share/zoneinfo"
#define TZLINK    "/etc/localtime"
#define TZFILE    "/etc/timezone"
#define CCFILE    TZROOT "/zone1970.tab"
#define CNFILE    TZROOT "/iso3166.tab"

namespace sysconfig::native
{
    using CountryMap = core::types::ValueMap<std::string, std::string>;
    using TimeZoneMap = core::types::ValueMap<std::string, TimeZoneCanonicalSpec>;

    class PosixTimeZoneProvider : public TimeZoneInterface
    {
        using This = PosixTimeZoneProvider;
        using Super = TimeZoneInterface;

    protected:
        PosixTimeZoneProvider(const std::string &name = "PosixTimeZoneProvider");

    public:
        bool is_pertinent() const override;
        void initialize() override;
        void deinitialize() override;

    protected:
        //==========================================================================
        // Time zone configuration

        TimeZoneAreas list_timezone_areas() const override;

        TimeZoneCountries list_timezone_countries(
            const TimeZoneArea &area) override;

        TimeZoneRegions list_timezone_regions(
            const TimeZoneLocationFilter &filter) override;

        TimeZoneCanonicalSpecs list_timezone_specs(
            const TimeZoneLocationFilter &filter) const override;

        TimeZoneCanonicalSpec get_timezone_spec(
            const TimeZoneCanonicalName &zone = {}) const override;

        TimeZoneInfo get_timezone_info(
            const TimeZoneCanonicalName &canonical_zone = {},
            const core::dt::TimePoint &timepoint = {}) const override;

        TimeZoneInfo set_timezone(
            const TimeZoneCanonicalName &zone) override;

        TimeZoneInfo set_timezone(
            const TimeZoneLocation &location) override;

    protected:
        std::string get_configured_zonename() const;
        void set_configured_zonename(const std::string &zone);

        TimeZoneCanonicalSpecs load_zones(const fs::path &zonetab = CCFILE) const;

        TimeZoneMap load_zone_map(const fs::path &zonetab = CCFILE) const;

        TimeZoneCanonicalSpec build_canonical_spec(
            const TimeZoneCanonicalName &zonename,
            const TimeZoneArea &area,
            const std::vector<TimeZoneCountryCode> &country_codes,
            const std::string &description,
            int latitude,
            int longitude,
            const CountryMap &country_name_map) const;

        void prune_redundant_regions(TimeZoneMap *zonemap) const;

        CountryMap load_countries(const fs::path &cctab = CNFILE) const;

        bool filter_includes_zone(const TimeZoneLocationFilter &filter,
                                  const TimeZoneCanonicalSpec &spec) const;

        bool country_match(const TimeZoneCountry &filter,
                           const TimeZoneCountry &candidate) const;

        int to_scalar_coord(const std::string &coord) const;

    protected:
        TimeZoneMap zone_map;
        core::SettingsStore zone_settings;
    };
}  // namespace sysconfig::native
