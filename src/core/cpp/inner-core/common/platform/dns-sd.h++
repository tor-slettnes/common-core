/// -*- c++ -*-
//==============================================================================
/// @file dns-sd.h++
/// @brief DNS service discovery interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "settings/settingsstore.h++"

namespace core::platform
{
    /// @brief Abstract provider for service discovery related functions
    class ServiceDiscoveryProvider : public Provider
    {
        using This = ServiceDiscoveryProvider;
        using Super = Provider;

    public:
        using AttributeMap = std::unordered_map<std::string, std::string>;

    protected:
        using Super::Super;

    public:
        virtual void add_service(
            const std::string &name,
            const std::string &type,
            uint port,
            const AttributeMap &attributes = {}) = 0;

        virtual void add_service_subtype(
            const std::string &name,
            const std::string &type,
            const std::string &subtype) = 0;

        virtual void commit() = 0;

        virtual void reset() = 0;
    };

    extern ProviderProxy<ServiceDiscoveryProvider> dns_sd;
}  // namespace core::platform
