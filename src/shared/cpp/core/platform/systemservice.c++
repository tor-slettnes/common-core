/// -*- c++ -*-
//==============================================================================
/// @file systemservice.c++
/// @brief System service related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "systemservice.h++"

namespace shared::platform
{
    ProviderProxy<SystemServiceProvider> system_service("system service");

    SystemServiceProvider::SystemServiceProvider(
        const std::string &provider_name,
        const std::string &service_name)
        : Super(provider_name),
          service_name_(service_name)
    {
    }

    std::string SystemServiceProvider::service_name() const noexcept
    {
        return this->service_name_;
    }

}  // namespace shared::platform
