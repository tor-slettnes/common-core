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
#include "buildinfo.h++"

#include <iomanip>
#include <sstream>

namespace core::messaging
{
    //==========================================================================
    // @class Endpoint

    Endpoint::Endpoint(const std::string &messaging_flavor,
                       const std::string &endpoint_type,
                       const std::string &channel_name,
                       const std::string &profile_name)
        : messaging_flavor_(messaging_flavor),
          endpoint_type_(endpoint_type),
          channel_name_(channel_name),
          profile_name_(profile_name),
          signal_handle_(messaging_flavor + "/" + endpoint_type + "/" + channel_name),
          settings_(SettingsStore::create_shared()),
          initialized_(false)
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
        this->initialized_ = true;
    }

    void Endpoint::deinitialize()
    {
        core::platform::signal_shutdown.disconnect(
            this->signal_handle_);
        this->initialized_ = false;
    }

    bool Endpoint::initialized() const
    {
        return this->initialized_;
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

    std::string Endpoint::profile_name() const
    {
        return this->profile_name_;
    }

    std::shared_ptr<SettingsStore> Endpoint::settings() const
    {
        if (!this->settings_->loaded())
        {
            for (const std::string &component : {
                     this->channel_name(),
                     std::string(PRODUCT_NAME),
                     std::string(PROJECT_NAME),
                     "common"s,
                 })
            {
                if (const std::optional<fs::path> &path = this->settings_file(component))
                {
                    logf_trace("%s loading settings from %s", *this, path.value());
                    this->settings_->load(path.value());
                }
            }
        }
        return this->settings_;
    }

    types::Value Endpoint::setting(const std::string &key,
                                   const types::Value &fallback) const
    {
        std::string profile = !this->profile_name().empty()
                                ? this->profile_name()
                                : this->channel_name();

        if (auto opt_value = this->settings()->get(profile).try_get(key))
        {
            return opt_value.value();
        }

        if (auto opt_value = this->settings()->get("_default_").try_get(key))
        {
            return opt_value.value();
        }

        return fallback;
    }

    std::optional<fs::path> Endpoint::settings_file(const std::string &flavor) const
    {
        if (!flavor.empty())
        {
            return str::format("%,s-endpoints-%,s",
                               this->messaging_flavor(),
                               flavor);
        }
        else
        {
            return {};
        }
    }

    void Endpoint::to_stream(std::ostream &stream) const
    {
        stream << this->messaging_flavor()
               << " "
               << this->channel_name()
               << " "
               << this->endpoint_type();
    }

}  // namespace core::messaging
