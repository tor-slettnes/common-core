/// -*- c++ -*-
//==============================================================================
/// @file systemservice.c++
/// @brief System service related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "systemservice.h++"

namespace core::platform
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

}  // namespace core::platform
