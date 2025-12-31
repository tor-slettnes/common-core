// -*- c++ -*-
//==============================================================================
/// @file sysconfig-posix-timezone.c++
/// @brief SysConfig service - Time Zone Configuration POSIX Implementation
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-posix-timezone.h++"
#include "status/exceptions.h++"

#include <fstream>

namespace sysconfig::native
{
    constexpr auto SETTINGS_FILE = "timezones";
    constexpr auto SETTING_AREA_NAMES = "area names";
    constexpr auto SETTING_AREA_ORDER = "area order";
    constexpr auto SETTING_COUNTRY_ORDER = "country order";
    constexpr auto SETTING_REGION_ORDER = "region order";

    //==========================================================================
    // Time zone configuration

    PosixTimeZoneProvider::PosixTimeZoneProvider(const std::string &name)
        : Super(name),
          zone_settings(SETTINGS_FILE)
    {
    }

    bool PosixTimeZoneProvider::is_pertinent() const
    {
        return fs::exists(CCFILE) && fs::exists(CNFILE);
    }

    void PosixTimeZoneProvider::initialize()
    {
        Super::initialize();
        this->zone_map = this->load_zone_map();
        signal_tzspec.emit(this->get_timezone_spec());
        signal_tzinfo.emit(this->get_timezone_info());
    }

    void PosixTimeZoneProvider::deinitialize()
    {
        Super::deinitialize();
    }

    TimeZoneAreas PosixTimeZoneProvider::list_timezone_areas() const
    {
        std::set<TimeZoneArea> areas;
        for (const auto &[name, spec] : this->zone_map)
        {
            areas.insert(spec.area);
        }

        // Start building our response
        TimeZoneAreas area_list;
        area_list.reserve(areas.size());

        // Determine if there is a matching "area order"  in "timezones.json".
        // If so, extract these areas in order from our initial set.
        core::types::Value area_order = this->zone_settings.get(SETTING_AREA_ORDER);
        for (const core::types::Value &priority_area : area_order.get_valuelist())
        {
            if (auto nh = areas.extract(priority_area.as_string()))
            {
                area_list.push_back(nh.value());
            }
        }

        // Populate any remaining areas. These are implicitly alphabetized.
        area_list.insert(area_list.end(), areas.begin(), areas.end());
        return area_list;
    }

    TimeZoneCountries PosixTimeZoneProvider::list_timezone_countries(const TimeZoneArea &area)
    {
        std::set<TimeZoneCountry> countries;

        for (const auto &[name, spec] : this->zone_map)
        {
            if (area.empty() || (spec.area == area))
            {
                for (const TimeZoneLocation &location : spec.locations)
                {
                    countries.insert(location.country);
                }
            }
        }

        // Start building our response
        TimeZoneCountries country_list;
        country_list.reserve(countries.size());

        // Determine if there is a matching "counry order" for this area in
        // "timezones.json".  If so, extract these regions in order from our
        // initial set.

        core::types::Value country_order = this->zone_settings.get(SETTING_COUNTRY_ORDER);
        for (const core::types::Value &priority_country : country_order.get(area).get_valuelist())
        {
            std::string next_country = priority_country.as_string();

            for (auto it = countries.begin(); it != countries.end(); it++)
            {
                if (it->name == next_country)
                {
                    country_list.push_back(std::move(*it));
                    countries.erase(it);
                    break;
                }
            }
        }

        // Finally, populate any remaining regions from our initial set.
        // These are implicitly alphabetized.
        country_list.insert(country_list.end(), countries.begin(), countries.end());
        return country_list;
    }

    TimeZoneRegions PosixTimeZoneProvider::list_timezone_regions(
        const TimeZoneLocationFilter &filter)
    {
        std::set<TimeZoneRegion> regions;

        // Determine the fully country name, even if we were supplied with a
        // 2-letter ISO code.  We use this for filtering, and we also need it
        // for looking up priority order below.

        std::string country_name = filter.country.name;
        if (country_name.empty())
        {
            CountryMap ccmap = this->load_countries();
            country_name = ccmap.get(filter.country.code);
        }

        // Populate regions (where they exist) from matching country lists.
        for (const auto &[name, spec] : this->zone_map)
        {
            if (filter.area.empty() || (filter.area == spec.area))
            {
                for (const TimeZoneLocation &location : spec.locations)
                {
                    if ((location.country.name == country_name) && !location.region.empty())
                    {
                        regions.insert(location.region);
                    }
                }
            }
        }

        // Start building our response
        TimeZoneRegions region_list;
        region_list.reserve(regions.size());

        // Determine if there is a matching "region order" for this country in "timezones.json".
        // If so, extract these regions in order from our initial set.
        core::types::Value region_order = this->zone_settings.get(SETTING_REGION_ORDER);
        for (const core::types::Value &priority_region : region_order.get(country_name).get_valuelist())
        {
            if (auto nh = regions.extract(priority_region.as_string()))
            {
                region_list.push_back(nh.value());
            }
        }

        // Finally, populate any remaining regions from our initial set.
        // These are implicitly alphabetized.

        region_list.insert(region_list.end(), regions.begin(), regions.end());
        return region_list;
    }

    TimeZoneCanonicalSpecs PosixTimeZoneProvider::list_timezone_specs(
        const TimeZoneLocationFilter &filter) const
    {
        TimeZoneCanonicalSpecs specs;
        for (const TimeZoneCanonicalSpec &spec : this->load_zones())
        {
            if (this->filter_includes_zone(filter, spec))
            {
                specs.push_back(spec);
            }
        }
        return specs;
    }

    TimeZoneCanonicalSpec PosixTimeZoneProvider::get_timezone_spec(
        const std::string &zonename) const
    {
        return this->zone_map.get(zonename.empty()
                                      ? this->get_configured_zonename()
                                      : zonename);
    }

    TimeZoneInfo PosixTimeZoneProvider::set_timezone(const TimeZoneCanonicalName &zonename)
    {
        this->set_configured_zonename(zonename);
        signal_tzspec.emit(this->get_timezone_spec(zonename));

        TimeZoneInfo zi = core::dt::tzinfo();
        signal_tzinfo.emit(zi);
        return zi;
    }

    TimeZoneInfo PosixTimeZoneProvider::set_timezone(const TimeZoneLocation &location)
    {
        TimeZoneLocationFilter filter = {
            .country = location.country,
        };

        for (const TimeZoneCanonicalSpec &spec : this->list_timezone_specs(filter))
        {
            for (const TimeZoneLocation &candidate : spec.locations)
            {
                if ((this->country_match(location.country, candidate.country)) && (location.region == candidate.region))
                {
                    return this->set_timezone(spec.name);
                }
            }
        }

        core::types::KeyValueMap missing_country = {
            {"code", location.country.code},
            {"name", location.country.name},
        };

        core::types::KeyValueMap missing_location = {
            {"country", missing_country.filtered()},
            {"region", location.region},
        };

        throw core::exception::NotFound(
            "No match for country/region in timezone database.",
            missing_location.filtered());
    }

    TimeZoneInfo PosixTimeZoneProvider::get_timezone_info(
        const TimeZoneCanonicalName &canonical_zone,
        const core::dt::TimePoint &timepoint) const

    {
        core::dt::TimePoint effective_time =
            (timepoint == core::dt::TimePoint())
                ? core::dt::Clock::now()
                : timepoint;

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
        std::string zonename;

        if (fs::is_symlink(TZLINK))
        {
            fs::path zone_path = fs::path(TZLINK).parent_path() / fs::read_symlink(TZLINK);
            zonename = zone_path.lexically_normal().lexically_relative(TZROOT).string();
        }
        else if (fs::is_regular_file(TZFILE))
        {
            std::ifstream zonefile(TZFILE);
            std::string zone;
            zonefile >> zonename;
        }

        return zonename;
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
        TimeZoneCanonicalSpecs tzlist;
        tzlist.reserve(this->zone_map.size());

        for (auto &[name, spec] : this->zone_map)
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
        core::types::Value area_names = this->zone_settings.get(SETTING_AREA_NAMES);

        std::ifstream zonefile(zonetab);
        for (char input[READCHUNK]; zonefile.getline(input, READCHUNK);)
        {
            std::smatch match;
            std::string line(input);
            if (std::regex_match(line, match, rx))
            {
                std::vector<std::string> cclist = core::str::split(match.str(1), ",");
                std::string zonename = match.str(4);
                std::string areaname = area_names.get(match.str(5), match.str(5)).as_string();
                std::string comment = match.str(7);
                int seconds_north = this->to_scalar_coord(match.str(2));
                int seconds_east = this->to_scalar_coord(match.str(3));

                zonemap[zonename] = this->build_canonical_spec(
                    zonename,       // zonename
                    areaname,       // area
                    cclist,         // country_codes
                    comment,        // description
                    seconds_north,  // latitude
                    seconds_east,   // longitude
                    ccmap);         // country_code_map
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

        spec.locations.reserve(country_codes.size());
        for (const TimeZoneCountryCode &cc : country_codes)
        {
            TimeZoneLocation &loc = spec.locations.emplace_back();
            loc.country = {
                .code = cc,
                .name = country_name_map.get(cc),
            };
            loc.region = description;
        }

        return spec;
    }

    void PosixTimeZoneProvider::prune_redundant_regions(TimeZoneMap *zonemap) const
    {
        std::unordered_map<TimeZoneCanonicalName, uint> repeats;

        // First, count the number of occurences per country
        for (auto &[name, spec] : *zonemap)
        {
            for (TimeZoneLocation &location : spec.locations)
            {
                const std::string &country_code = location.country.code;
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
            for (TimeZoneLocation &location : spec.locations)
            {
                const std::string &cc = location.country.code;
                if (repeats.at(cc) < 2)
                {
                    location.region.clear();
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

    bool PosixTimeZoneProvider::filter_includes_zone(
        const TimeZoneLocationFilter &filter,
        const TimeZoneCanonicalSpec &spec) const
    {
        if (!filter.area.empty() && (spec.area != filter.area))
        {
            return false;
        }

        if (filter.country.code.empty() && filter.country.name.empty())
        {
            return true;
        }

        for (const TimeZoneLocation &location : spec.locations)
        {
            if (this->country_match(filter.country, location.country))
            {
                return true;
            }
        }

        return false;
    }

    bool PosixTimeZoneProvider::country_match(const TimeZoneCountry &filter,
                                              const TimeZoneCountry &candidate) const
    {
        return (filter.code == candidate.code) || (filter.name == candidate.name);
    }

    int PosixTimeZoneProvider::to_scalar_coord(const std::string &coord) const
    {
        int deg = std::stol(coord.substr(0, 3));
        uint min = (coord.size() > 3) ? std::stoul(coord.substr(3, 2)) : 0;
        uint sec = (coord.size() > 5) ? std::stoul(coord.substr(5, 2)) : 0;
        return (3600 * deg) + (60 * min) + sec;
    }

}  // namespace sysconfig::native
