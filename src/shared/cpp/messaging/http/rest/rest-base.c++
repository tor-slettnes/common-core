/// -*- c++ -*-
//==============================================================================
/// @file rest-base.c++
/// @brief Implements REST request/reply pattern - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rest-base.h++"
#include "string/convert.h++"

namespace shared::http
{
    RESTBase::RESTBase(const std::string &endpoint_type,
                       const std::string &service_name)
        : messaging::Endpoint("REST", endpoint_type, service_name)
    {
    }

    fs::path RESTBase::settings_file(const std::string &product) const
    {
        return str::format("%,s-services-%,s.json",
                           this->messaging_flavor(),
                           product);
    }

    std::string RESTBase::real_url(const std::string &partial_url,
                                   const std::string &scheme_option,
                                   const std::string &host_option,
                                   const std::string &port_option,
                                   const std::string &location_option,
                                   const std::string &default_scheme,
                                   const std::string &default_host,
                                   uint default_port,
                                   const std::string &default_location) const
    {
        std::string scheme, host, location;
        uint port = 0;
        this->spliturl(partial_url, &scheme, &host, &port, &location);
        if (scheme.empty())
        {
            scheme = this->setting(scheme_option, default_scheme).as_string();
        }
        if (host.empty())
        {
            host = this->setting(host_option, default_host).as_string();
        }
        if (port == 0)
        {
            port = this->setting(port_option, default_port).as_uint();
        }
        if (location.empty())
        {
            location = this->setting(location_option, default_location).as_string();
        }
        return this->joinurl(scheme, host, port, location);
    }

    bool RESTBase::spliturl(const std::string &url,
                            std::string *scheme,
                            std::string *host,
                            uint *port,
                            std::string *location) const
    {
        static const std::regex rx(
            "(?:(\\w+)://)?"                      // scheme
            "(\\[[0-9A-Fa-f:]+\\]|[\\w\\-\\.]+)"  // `[ip6::address]` or `f.q.d.n`
            "(?::(\\d+))?"                        // port number
            "(?:/|(/.+))?$"                       // location
        );
        std::smatch match;
        if (std::regex_match(url, match, rx))
        {
            *scheme = match.str(1);
            *host = match.str(2);
            *port = str::convert_to<uint>(match.str(3));
            *location = match.str(4);
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string RESTBase::joinurl(const std::string &scheme,
                                  const std::string &host,
                                  uint port,
                                  const std::string &location) const
    {
        std::stringstream ss;
        ss << ((scheme.size() > 0) ? scheme
               : port == 443       ? "https"s
                                   : "http"s)
           << "://"
           << (host.empty() ? "localhost"s : host);

        if (port != 0)
        {
            ss << ":" << port;
        }

        ss << location;
        return ss.str();
    }

}  // namespace shared::http
