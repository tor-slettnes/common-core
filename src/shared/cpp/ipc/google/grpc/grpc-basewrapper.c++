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

/// C stdlib
#include <netdb.h>

#include <memory>

namespace cc::grpc
{
    std::shared_ptr<SettingsStore> service_settings;

    void init_service_settings()
    {
        if (!service_settings)
        {
            service_settings = SettingsStore::create_shared(
                types::PathList({SETTINGS_FILE_PRODUCT, SETTINGS_FILE_COMMON}));
        }
    }

    //==========================================================================
    // WrapperBase

    WrapperBase::WrapperBase(const std::string &fullServiceName)
        : fullServiceName_(fullServiceName)
    {
        init_service_settings();
        logf_trace("Instantiated %r service wrapper with settings: %s",
                   this->servicename(),
                   *service_settings);
    }

    std::string WrapperBase::realaddress(const std::string &address,
                                         const std::string &hostOption,
                                         const std::string &portOption,
                                         std::string defaultHost,
                                         unsigned int defaultPort) const
    {
        std::string personality;
        std::string host;
        uint port;
        this->splitaddress(address, &personality, &host, &port);

        if (host.empty())
        {
            host = this->setting(hostOption, personality).as_string();
        }
        if (host.empty())
        {
            host = defaultHost;
        }

        if (port == 0)
        {
            port = this->setting(portOption, personality).as_uint();
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
        return this->setting(MAX_MESSAGE_SIZE, {}, 0).as_uint();
    }

    std::string WrapperBase::servicename(bool full) const
    {
        return full ? this->fullServiceName_ : str::stem(this->fullServiceName_, ":");
    }

    types::Value WrapperBase::setting(const std::string &key,
                                      const std::string &personality,
                                      const types::Value &defaultValue) const
    {
        types::Value value;
        if (!personality.empty())
        {
            value = service_settings->get(PERSONALITY_SECTION, {})
                        .get(personality, {})
                        .get(key, {});
        }

        if (value.empty())
        {
            value = service_settings->get(DEFAULT_SECTION, {})
                        .get(this->servicename(), {})
                        .get(key, defaultValue);
        }

        return value;
    }

    void WrapperBase::splitaddress(const std::string &address,
                                   std::string *personality,
                                   std::string *host,
                                   uint *port) const
    {
        static const std::regex rx(
            "(?:(\\w*)@)?"                  // personality
            "(\\[[\\w\\.:]*\\]|[\\w\\.]*)"  // host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?$"                 // port
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *personality = match.str(1);
            *host = match.str(2);
            *port = match.length(3) ? std::stoi(match.str(3)) : 0;
        }
        else
        {
            personality->clear();
            host->clear();
            *port = 0;
        }
    }

    std::string WrapperBase::joinaddress(const std::string &host, uint port) const
    {
        return host + ":" + std::to_string(port);
    }
}  // namespace cc::grpc
