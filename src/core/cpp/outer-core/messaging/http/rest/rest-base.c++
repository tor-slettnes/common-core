/// -*- c++ -*-
//==============================================================================
/// @file rest-base.c++
/// @brief Implements REST request/reply pattern - base
/// @author Tor Slettnes
//==============================================================================

#include "rest-base.h++"
#include "string/convert.h++"

namespace cc::http
{
    std::string RESTBase::real_url(const std::string &partial_url,
                                   const std::string &url_option,
                                   const std::string &scheme_option,
                                   const std::string &username_option,
                                   const std::string &password_option,
                                   const std::string &host_option,
                                   const std::string &port_option,
                                   const std::string &path_option,
                                   const std::string &default_scheme,
                                   const std::string &default_host,
                                   uint default_port,
                                   const std::string &default_path) const
    {
        std::string scheme, username, password, host, path;
        uint port = 0;

        std::string url = !partial_url.empty()
                            ? partial_url
                            : this->setting(url_option).as_string();

        this->spliturl(url, &scheme, &username, &password, &host, &port, &path);

        if (scheme.empty())
        {
            scheme = this->setting(scheme_option, default_scheme).as_string();
        }
        if (username.empty())
        {
            username = this->setting(username_option).as_string();
        }
        if (password.empty())
        {
            password = this->setting(password_option).as_string();
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
        return this->joinurl(scheme, username, password, host, port, path);
    }

    bool RESTBase::spliturl(const std::string &url,
                            std::string *scheme,
                            std::string *username,
                            std::string *password,
                            std::string *host,
                            uint *port,
                            std::string *path) const
    {
        static const std::regex rx(
            "(?:(\\w+)://)?"                      // (1) scheme
            "(?:(\\w+)"                           // (2) username
            "(?::(.+))?@)?"                       // (3) password
            "(\\[[0-9A-Fa-f:]+\\]|[\\w\\-\\.]+)"  // (4) `[ip6::address]` or `f.q.d.n`
            "(?::(\\d+))?"                        // (5) port number
            "(/.*)?$"                             // (6) path
        );
        std::smatch match;
        if (std::regex_match(url, match, rx))
        {
            *scheme = match.str(1);
            *username = match.str(2);
            *password = match.str(3);
            *host = match.str(4);
            *port = core::str::convert_to<uint>(match.str(5), 0);
            *path = match.str(6);
            return true;
        }
        else
        {
            return false;
        }
    }

    std::string RESTBase::joinurl(const std::string &scheme,
                                  const std::string &username,
                                  const std::string &password,
                                  const std::string &host,
                                  uint port,
                                  const std::string &path) const
    {
        std::stringstream ss;
        ss << ((scheme.size() > 0) ? scheme
               : port == 443       ? "https"s
                                   : "http"s);

        ss << "://";

        if (!username.empty())
        {
            ss << username;
            if (!password.empty())
            {
                ss << ":"
                   << password;
            }
            ss << "@";
        }

        ss << (host.empty() ? "localhost"s : host);

        if (port != 0)
        {
            ss << ":" << port;
        }

        ss << path;
        return ss.str();
    }

}  // namespace cc::http
