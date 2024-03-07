/// -*- c++ -*-
//==============================================================================
/// @file rest-base.c++
/// @brief Implements REST request/reply pattern - base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rest-base.h++"
#include "string/convert.h++"

namespace core::http
{
    std::string RESTBase::real_url(const std::string &partial_url,
                                   const std::string &scheme_option,
                                   const std::string &host_option,
                                   const std::string &port_option,
                                   const std::string &path_option,
                                   const std::string &default_scheme,
                                   const std::string &default_host,
                                   uint default_port,
                                   const std::string &default_path) const
    {
        std::string scheme, host, path;
        uint port = 0;
        this->spliturl(partial_url, &scheme, &host, &port, &path);
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
        if (path.empty())
        {
            path = this->setting(path_option, default_path).as_string();
        }
        return this->joinurl(scheme, host, port, path);
    }

    bool RESTBase::spliturl(const std::string &url,
                            std::string *scheme,
                            std::string *host,
                            uint *port,
                            std::string *path) const
    {
        static const std::regex rx(
            "(?:(\\w+)://)?"                      // scheme
            "(\\[[0-9A-Fa-f:]+\\]|[\\w\\-\\.]+)"  // `[ip6::address]` or `f.q.d.n`
            "(?::(\\d+))?"                        // port number
            "(/.*)?$"                             // path
        );
        std::smatch match;
        if (std::regex_match(url, match, rx))
        {
            *scheme = match.str(1);
            *host = match.str(2);
            *port = str::convert_to<uint>(match.str(3), 0);
            *path = match.str(4);
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
                                  const std::string &path) const
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

        ss << path;
        return ss.str();
    }

}  // namespace core::http
