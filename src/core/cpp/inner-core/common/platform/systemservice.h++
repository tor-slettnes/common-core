/// -*- c++ -*-
//==============================================================================
/// @file systemservice.h++
/// @brief System service related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "provider.h++"
#include "types/filesystem.h++"

namespace core::platform
{
    /// @brief Abstract provider for runtime related functions
    class SystemServiceProvider : public Provider
    {
        using This = SystemServiceProvider;
        using Super = Provider;

    protected:
        SystemServiceProvider(
            const std::string &provider_name,
            const std::string &service_name);

    public:
        virtual std::string service_name() const noexcept;

        virtual void install() {}
        virtual void uninstall() {}

        virtual void start() {}
        virtual void stop() {}

    private:
        std::string service_name_;
    };

    extern ProviderProxy<SystemServiceProvider> system_service;
}  // namespace core::platform
