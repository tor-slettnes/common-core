/// -*- c++ -*-
//==============================================================================
/// @file host.h++
/// @brief Host related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "settings/settingsstore.h++"

namespace core::platform
{
    /// @brief Abstract provider for host related functions
    class HostProvider : public Provider
    {
        using This = HostProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        virtual void set_host_name(const std::string &hostname) = 0;
        virtual std::string get_host_name() const = 0;

        virtual std::string get_system_name() const;
        virtual std::string get_system_version() const;

        virtual std::string get_system_release_flavor_short() const;
        virtual std::string get_system_release_flavor_long() const;

        virtual std::string get_system_release_version_id() const;
        virtual std::string get_system_release_version_name() const;

        // virtual std::string get_machine_architecture() const;
        // virtual std::string get_machine_cpu() const;
        virtual void reboot() = 0;
    };

    extern ProviderProxy<HostProvider> host;
}  // namespace core::platform
