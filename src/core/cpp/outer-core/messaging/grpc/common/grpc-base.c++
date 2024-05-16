/// -*- c++ -*-
//==============================================================================
/// @file grpc-base.c++
/// @brief Common wrapper functionality for gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-base.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"
#include "settings/settingsstore.h++"
#include "string/misc.h++"
#include "buildinfo.h"  // PROJECT_NAME

/// C stdlib
#include <netdb.h>

#include <memory>

namespace cc::grpc
{
    //==========================================================================
    // Base

    Base::Base(const std::string &endpoint_type,
               const std::string &full_service_name)
        : Super("gRPC", endpoint_type, str::stem(full_service_name, ".")),
          full_service_name_(full_service_name)
    {
    }

    std::string Base::servicename(bool full) const
    {
        return full ? this->full_service_name_
                    : str::stem(this->full_service_name_, ".");
    }

    std::string Base::realaddress(const std::string &address,
                                  const std::string &hostOption,
                                  const std::string &portOption,
                                  std::string defaultHost,
                                  uint defaultPort) const
    {
        std::string host;
        uint port = 0;

        this->splitaddress(address, &host, &port);

        if (host.empty())
        {
            host = this->setting(hostOption, defaultHost).as_string();
        }

        if (port == 0)
        {
            port = this->setting(portOption, defaultPort).as_uint();
        }

        return this->joinaddress(host, port);
    }

    uint Base::max_request_size() const
    {
        return this->setting(MAX_REQUEST_SIZE, 0).as_uint();
    }

    uint Base::max_reply_size() const
    {
        return this->setting(MAX_REPLY_SIZE, 0).as_uint();
    }

    void Base::splitaddress(const std::string &address,
                            std::string *host,
                            uint *port) const
    {
        static const std::regex rx(
            "(\\[[\\w\\.:]*\\]|[\\w\\.]*)"  // host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?$"                 // port
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *host = match.str(1);
            *port = match.length(2) ? std::stoi(match.str(2)) : 0;
        }
        else
        {
            host->clear();
            *port = 0;
        }
    }

    std::string Base::joinaddress(const std::string &host, uint port) const
    {
        if (port != 0)
        {
            return host + ":" + std::to_string(port);
        }
        else
        {
            return "";
        }
    }

}  // namespace cc::grpc
