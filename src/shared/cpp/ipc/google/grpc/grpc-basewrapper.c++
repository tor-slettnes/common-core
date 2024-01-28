/// -*- c++ -*-
//==============================================================================
/// @file grpc-basewrapper.c++
/// @brief Common wrapper functionality for gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-basewrapper.h++"
#include "chrono/date-time.h++"
#include "status/exceptions.h++"
#include "config/settingsstore.h++"
#include "string/misc.h++"
#include "buildinfo.h"  // PROJECT_NAME

/// C stdlib
#include <netdb.h>

#include <memory>

namespace cc::grpc
{
    constexpr auto SETTINGS_FILE_COMMON = "grpc-endpoints-common";
    constexpr auto SETTINGS_FILE_PRODUCT = "grpc-endpoints-" PROJECT_NAME;

    //==========================================================================
    // WrapperBase

    WrapperBase::WrapperBase(const std::string &fullServiceName)
        : fullServiceName_(fullServiceName)
    {
        logf_trace("Instantiated %r wrapper with settings: %s",
                   this->servicename(),
                   *WrapperBase::settings());
    }

    std::shared_ptr<SettingsStore> WrapperBase::settings()
    {
        if (WrapperBase::settings_)
        {
            return WrapperBase::settings_;
        }
        else
        {
            return WrapperBase::settings_ = SettingsStore::create_shared(
                       types::PathList({SETTINGS_FILE_PRODUCT,
                                        SETTINGS_FILE_COMMON}));
        }
    }

    std::string WrapperBase::realaddress(const std::string &address,
                                         const std::string &hostOption,
                                         const std::string &portOption,
                                         std::string defaultHost,
                                         uint defaultPort) const
    {
        std::string host;
        uint port;
        this->splitaddress(address, &host, &port);

        if (host.empty())
        {
            host = this->setting(hostOption).as_string();
        }
        if (host.empty())
        {
            host = defaultHost;
        }

        if (port == 0)
        {
            port = this->setting(portOption).as_uint();
        }

        if (port == 0)
        {
            std::string netservice = str::tolower(this->servicename());
            struct servent *ent = getservbyname(netservice.c_str(), NULL);
            if (ent != NULL)
            {
                port = ent->s_port;
            }
            endservent();
        }
        if (port == 0)
        {
            port = defaultPort;
        }

        return this->joinaddress(host, port);
    }

    void WrapperBase::require(const std::string &fieldname,
                              const std::string &value) const
    {
        if (value.empty())
        {
            throwf(exception::MissingArgument, ("%r is required", fieldname));
        }
    }

    int WrapperBase::max_message_size() const
    {
        return this->setting(MAX_MESSAGE_SIZE, 0).as_uint();
    }

    std::string WrapperBase::servicename(bool full) const
    {
        return full ? this->fullServiceName_ : str::stem(this->fullServiceName_, ".");
    }

    types::Value WrapperBase::setting(const std::string &key,
                                      const types::Value &defaultValue) const
    {
        logf_trace("Getting setting from %s: %r -> %r",
                   WrapperBase::settings()->filename(),
                   this->servicename(),
                   key);
        return WrapperBase::settings()
            ->get(this->servicename())
            .get(key, defaultValue);
    }

    void WrapperBase::splitaddress(const std::string &address,
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

    std::string WrapperBase::joinaddress(const std::string &host, uint port) const
    {
        return host + ":" + std::to_string(port);
    }

    std::shared_ptr<SettingsStore> WrapperBase::settings_;

}  // namespace cc::grpc
