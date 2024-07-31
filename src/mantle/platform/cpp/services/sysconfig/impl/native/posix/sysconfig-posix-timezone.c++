// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-timezone.c++
/// @brief SysConfig service - Time Zone Configuration POSIX Implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-posix-timezone.h++"
#include <fstream>

namespace platform::sysconfig::native
{
    //==========================================================================
    // Time zone configuration

    PosixTimeZoneProvider::PosixTimeZoneProvider()
        : Super("PosixTimeZoneProvider")
    {
    }

    bool PosixTimeZoneProvider::is_pertinent()
    {
        return fs::exists(CCFILE) && fs::exists(CNFILE);
    }

    void PosixTimeZoneProvider::initialize()
    {
        Super::initialize();
        signal_tzconfig.emit(this->get_timezone_config());
        signal_tzinfo.emit(this->get_timezone_info());
    }

    void PosixTimeZoneProvider::deinitialize()
    {
        Super::deinitialize();
    }

    TimeZoneAreas PosixTimeZoneProvider::list_timezone_areas() const
    {
        std::set<TimeZoneArea> areas;
        for (const auto &[name, spec] : this->load_zone_map())
        {
            areas.insert(spec.area);
        }
        return {
            areas.begin(),
            areas.end(),
        };
    }

    TimeZoneCountries PosixTimeZoneProvider::list_timezone_countries(const TimeZoneArea &area)
    {
        std::set<TimeZoneCountry> countries;

        for (const auto &[name, spec] : this->load_zone_map())
        {
            if (area.empty() || (spec.area == area))
            {
                for (const TimeZoneCountryRegion &country_region : spec.countries)
                {
                    countries.insert(country_region.country);
                }
            }
        }

        return {
            countries.begin(),
            countries.end(),
        };
    }

    TimeZoneCanonicalSpecs PosixTimeZoneProvider::list_timezone_specs(
        const TimeZoneLocationFilter &filter) const
    {
        TimeZoneCanonicalSpecs specs;
        for (const TimeZoneCanonicalSpec &spec : this->load_zones())
        {
            if (filter.area.empty() || (spec.area == filter.area))
            {
                if (this->filter_empty_or_includes_country(
                        filter.country,
                        spec.countries))
                {
                    specs.push_back(spec);
                }
            }
        }
        return specs;
    }

    TimeZoneCanonicalSpec PosixTimeZoneProvider::get_timezone_spec(
        const std::string &zonename) const
    {
        TimeZoneMap tzmap = this->load_zone_map();
        return tzmap.get(zonename.empty() ? this->get_configured_zonename() : zonename);
    }

    TimeZoneInfo PosixTimeZoneProvider::set_timezone(const TimeZoneConfig &config)
    {
        this->set_configured_zonename(config.zonename);
        signal_tzconfig.emit(config);

        TimeZoneInfo zi = core::dt::tzinfo();
        signal_tzinfo.emit(zi);
        return zi;
    }

    TimeZoneConfig PosixTimeZoneProvider::get_timezone_config() const
    {
        return {
            .zonename = this->get_configured_zonename(),
            .automatic = false,
            .provider = "",
        };
    }

    TimeZoneInfo PosixTimeZoneProvider::get_timezone_info(
        const TimeZoneCanonicalName &canonical_zone,
        const core::dt::TimePoint &timepoint) const

    {
        core::dt::TimePoint effective_time =
            (timepoint == core::dt::TimePoint()) ? core::dt::Clock::now() : timepoint;

        if (canonical_zone.empty())
        {
            return core::dt::tzinfo(effective_time);
        }
        else
        {
            return core::dt::tzinfo(canonical_zone, effective_time);
        }
    }

    std::string PosixTimeZoneProvider::get_configured_zonename() const
    {
        if (fs::is_symlink(TZLINK))
        {
            return (fs::read_symlink(TZLINK).lexically_relative(TZROOT)).string();
        }
        else if (fs::is_regular_file(TZFILE))
        {
            std::ifstream zonefile(TZFILE);
            std::string zone;
            zonefile >> zone;
            return zone;
        }
        else
        {
            return {};
        }
    }

    void PosixTimeZoneProvider::set_configured_zonename(const std::string &zonename)
    {
        // Create/update /etc/localtime symlink
        if (fs::exists(fs::symlink_status(TZLINK)))
            fs::remove(TZLINK);
        fs::create_symlink(fs::path(TZROOT) / fs::path(zonename).relative_path(), TZLINK);

        // Write /etc/timzeone file
        std::ofstream zonefile(TZFILE);
        zonefile << zonename << std::endl;

        // Clear the contents of the TZ envirnonment variable for this process, if set.
        std::string tz = "TZ";
        putenv(tz.data());
        tzset();
    }

    TimeZoneCanonicalSpecs PosixTimeZoneProvider::load_zones(const fs::path &zonetab) const
    {
        TimeZoneMap tzmap = this->load_zone_map();
        TimeZoneCanonicalSpecs tzlist;
        tzlist.reserve(tzmap.size());

        for (auto &[name, spec] : tzmap)
        {
            tzlist.push_back(std::move(spec));
        }

        return tzlist;
    }

    TimeZoneMap PosixTimeZoneProvider::load_zone_map(const fs::path &zonetab) const
    {
        static const std::regex rx(
            "^([\\w,]+)"           // (1) country code(s)
            "\\t([\\+\\-]\\d+)"    // (2) latitude (north)
            "([\\+\\-]\\d+)"       // (3) longitude (east)
            "\\t((\\w+)/?(\\S*))"  // (4) zone name, (5) area, (6) location
            "\\t?([^#]*)?");       // (7) description

        CountryMap ccmap = this->load_countries();
        TimeZoneMap zonemap;

        std::ifstream zonefile(zonetab);
        for (char input[READCHUNK]; zonefile.getline(input, READCHUNK);)
        {
            std::smatch match;
            std::string line(input);
            if (std::regex_match(line, match, rx))
            {
                std::string zonename = match.str(4);
                zonemap[zonename] = this->build_canonical_spec(
                    zonename,                             // zonename
                    match.str(5),                         // area
                    core::str::split(match.str(1), ","),  // country_codes
                    match.str(7),                         // description
                    this->to_scalar_coord(match.str(2)),  // latitude
                    this->to_scalar_coord(match.str(3)),  // longitude
                    ccmap);                               // country_code_map
            }
        }

        this->prune_redundant_regions(&zonemap);
        return zonemap;
    }

    TimeZoneCanonicalSpec PosixTimeZoneProvider::build_canonical_spec(
        const TimeZoneCanonicalName &zonename,
        const TimeZoneArea &area,
        const std::vector<TimeZoneCountryCode> &country_codes,
        const std::string &description,
        int latitude,
        int longitude,
        const CountryMap &country_name_map) const
    {
        TimeZoneCanonicalSpec spec = {
            .name = zonename,
            .area = area,
            .latitude = latitude,
            .longitude = longitude,
        };

        spec.countries.reserve(country_codes.size());
        for (const TimeZoneCountryCode &cc : country_codes)
        {
            TimeZoneCountryRegion &cr = spec.countries.emplace_back();
            cr.country = {
                .code = cc,
                .name = country_name_map.get(cc),
            };
            cr.region = description;
        }

        return spec;
    }

    void PosixTimeZoneProvider::prune_redundant_regions(TimeZoneMap *zonemap) const
    {
        std::unordered_map<TimeZoneCanonicalName, uint> repeats;

        // First, count the number of occurences per country
        for (auto &[name, spec] : *zonemap)
        {
            for (TimeZoneCountryRegion &country_region : spec.countries)
            {
                const std::string &country_code = country_region.country.code;
                try
                {
                    ++repeats.at(country_code);
                }
                catch (const std::out_of_range)
                {
                    repeats.insert_or_assign(country_code, 1);
                }
            }
        }

        // Now remove region descriptions for countries with only one time zone
        for (auto &[name, spec] : *zonemap)
        {
            for (TimeZoneCountryRegion &country_region : spec.countries)
            {
                const std::string &country_code = country_region.country.code;
                if (repeats.at(country_code) < 2)
                {
                    country_region.region.clear();
                }
            }
        }
    }

    CountryMap PosixTimeZoneProvider::load_countries(const fs::path &cctab) const
    {
        static const std::regex rx(
            "^(\\w{2})\\t"  // (1) country code
            "([^#]*)");     // (2) country name

        CountryMap countrymap;
        std::ifstream ccfile(cctab);
        for (char input[READCHUNK]; ccfile.getline(input, READCHUNK);)
        {
            std::smatch match;
            std::string line(input);
            if (std::regex_match(line, match, rx))
            {
                countrymap[match.str(1)] = match.str(2);
            }
        }
        return countrymap;
    }

    bool PosixTimeZoneProvider::filter_empty_or_includes_country(
        const TimeZoneCountry &filter,
        const TimeZoneCountryRegions &candidates) const
    {
        if (filter.code.empty() && filter.name.empty())
        {
            return true;
        }

        for (const TimeZoneCountryRegion &candidate : candidates)
        {
            if ((filter.code == candidate.country.code) ||
                (filter.name == candidate.country.name))
            {
                return true;
            }
        }

        return false;
    }

    int PosixTimeZoneProvider::to_scalar_coord(const std::string &coord) const
    {
        int deg = std::stol(coord.substr(0, 3));
        uint min = (coord.size() > 3) ? std::stoul(coord.substr(3, 2)) : 0;
        uint sec = (coord.size() > 5) ? std::stoul(coord.substr(5, 2)) : 0;
        return (3600 * deg) + (60 * min) + sec;
    }

}  // namespace platform::sysconfig::native
