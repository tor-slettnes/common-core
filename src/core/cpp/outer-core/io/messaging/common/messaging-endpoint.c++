/// -*- c++ -*-
//==============================================================================
/// @file messaging-endpoint.c++
/// @brief Abstact base for a single service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "messaging-endpoint.h++"
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
          channel_name_(channel_name)
    {
    }

    Endpoint::~Endpoint()
    {
        this->deinitialize();
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
        try
        {
            return Endpoint::settings_map_.at(this->messaging_flavor());
        }
        catch (std::out_of_range)
        {
            auto [it, inserted] = Endpoint::settings_map_.insert_or_assign(
                this->messaging_flavor(),
                SettingsStore::create_shared(types::PathList({
                    this->settings_file(PROJECT_NAME),
                    this->settings_file("common"),
                })));
            return it->second;
        }
    }

    types::Value Endpoint::setting(const std::string &key,
                                   const types::Value &fallback) const
    {
        return this->settings()->get(this->channel_name()).get(key, fallback);
    }

    fs::path Endpoint::settings_file(const std::string &product) const
    {
        return str::format("%,s-endpoints-%,s.json",
                           this->messaging_flavor(),
                           product);
    }

    void Endpoint::to_stream(std::ostream &stream) const
    {
        str::format(stream,
                    "%s %r %s",
                    this->messaging_flavor(),
                    this->channel_name(),
                    this->endpoint_type());
    }

    std::map<std::string, std::shared_ptr<SettingsStore>> Endpoint::settings_map_;

}  // namespace core::messaging
