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
        signal_tzconfig.emit(this->get_configured_timezone());
        signal_tzinfo.emit(this->get_current_timezone());
    }

    void PosixTimeZoneProvider::deinitialize()
    {
        Super::deinitialize();
    }

    std::vector<TimeZoneSpec> PosixTimeZoneProvider::get_timezone_specs() const
    {
        return this->load_zones();
    }

    TimeZoneSpec PosixTimeZoneProvider::get_timezone_spec(const std::string &zonename) const
    {
        TimeZoneMap tzmap = this->load_zone_map();
        auto it = tzmap.find(zonename.empty() ? this->get_configured_zonename() : zonename);
        if (it != tzmap.end())
        {
            return it->second;
        }
        return {};
    }

    TimeZoneInfo PosixTimeZoneProvider::set_timezone(const TimeZoneConfig &config)
    {
        this->set_configured_zonename(config.zonename);
        signal_tzconfig.emit(config);

        TimeZoneInfo zi = core::dt::tzinfo();
        signal_tzinfo.emit(zi);
        return zi;
    }

    TimeZoneConfig PosixTimeZoneProvider::get_configured_timezone() const
    {
        return {
            .zonename = this->get_configured_zonename(),
            .automatic = false,
            .provider = "",
        };
    }

    TimeZoneInfo PosixTimeZoneProvider::get_current_timezone() const
    {
        return core::dt::tzinfo();
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

    TimeZoneList PosixTimeZoneProvider::load_zones(const fs::path &zonetab) const
    {
        TimeZoneMap tzmap = this->load_zone_map();
        TimeZoneList tzlist;
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
            "\\t((\\w+)/?(\\S*))"  // (4) zone name, (5) continent, (6) place
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
                std::vector<std::string> cclist = core::str::split(match.str(1), ",");
                int latitude = this->to_scalar_coord(match.str(2));
                int longitude = this->to_scalar_coord(match.str(3));
                std::string zonename = match.str(4);
                std::string continent = match.str(5);
                std::string description = match.str(7);

                // auto it = this->regionNames.find(regionname);
                // if (it != this->regionNames.end())
                //     regionname = it->second;

                for (const std::string &cc : cclist)
                {
                    zonemap.emplace(
                        zonename,
                        TimeZoneSpec{
                            .zonename = zonename,
                            .continent = continent,
                            .countrycode = cc,
                            .countryname = ccmap.get(cc),
                            .displayname = description,
                            .latitude = latitude,
                            .longitude = longitude,
                        });
                }
            }
        }
        return zonemap;
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

    int PosixTimeZoneProvider::to_scalar_coord(const std::string &coord) const
    {
        int deg = std::stol(coord.substr(0, 3));
        uint min = (coord.size() > 3) ? std::stoul(coord.substr(3, 2)) : 0;
        uint sec = (coord.size() > 5) ? std::stoul(coord.substr(5, 2)) : 0;
        return (3600 * deg) + (60 * min) + sec;
    }

}  // namespace platform::sysconfig::native
