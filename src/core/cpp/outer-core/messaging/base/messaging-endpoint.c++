/// -*- c++ -*-
//==============================================================================
/// @file messaging-endpoint.c++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "messaging-endpoint.h++"
#include "logging/logging.h++"
#include "string/stream.h++"
#include "platform/init.h++"
#include "buildinfo.h"

#include <iomanip>
#include <sstream>

namespace core::messaging
{
    //==========================================================================
    // @class Endpoint

    Endpoint::Endpoint(const std::string &messaging_flavor,
                       const std::string &endpoint_type,
                       const std::string &channel_name)
        : messaging_flavor_(messaging_flavor),
          endpoint_type_(endpoint_type),
          channel_name_(channel_name),
          signal_handle_(messaging_flavor + "/" + endpoint_type + "/" + channel_name),
          settings_(SettingsStore::create_shared())
    {
    }

    Endpoint::~Endpoint()
    {
        this->deinitialize();
    }

    void Endpoint::initialize()
    {
        core::platform::signal_shutdown.connect(
            this->signal_handle_,
            std::bind(&Endpoint::deinitialize, this));
    }

    void Endpoint::deinitialize()
    {
        core::platform::signal_shutdown.disconnect(
            this->signal_handle_);
    }

    std::string Endpoint::messaging_flavor() const
    {
        return this->messaging_flavor_;
    }

    std::string Endpoint::endpoint_type() const
    {
        return this->endpoint_type_;
    }

    std::string Endpoint::channel_name() const
    {
        return this->channel_name_;
    }

    std::shared_ptr<SettingsStore> Endpoint::settings() const
    {
        if (!this->settings_->loaded())
        {
            for (const std::string &component: {PRODUCT_NAME, PROJECT_NAME, "common"})
            {
                if (const std::optional<fs::path> &path = this->settings_file(component))
                {
                    logf_debug("%s loading settings from %s", *this, path);
                    this->settings_->load(path.value());
                }
            }
        }
        return this->settings_;
    }

    types::Value Endpoint::setting(const std::string &key,
                                   const types::Value &fallback) const
    {
        if (auto value = this->settings()->get(this->channel_name()).get(key))
        {
            return value;
        }
        else if (auto value = this->settings()->get("_default_").get(key))
        {
            return value;
        }
        else
        {
            return fallback;
        }
    }

    std::optional<fs::path> Endpoint::settings_file(const std::string &deployment_flavor) const
    {
        if (!deployment_flavor.empty())
        {
            return str::format("%,s-endpoints-%,s",
                               this->messaging_flavor(),
                               deployment_flavor);
        }
        else
        {
            return {};
        }
    }

    void Endpoint::to_stream(std::ostream &stream) const
    {
        str::format(stream,
                    "%s %r %s",
                    this->messaging_flavor(),
                    this->channel_name(),
                    this->endpoint_type());
    }

}  // namespace core::messaging
