// -*- c++ -*-
//==============================================================================
/// @file system-api-host.h++
/// @brief System service - Host Configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/provider.h++"
#include "system-types.h++"
#include "thread/signaltemplate.h++"

namespace platform::system
{
    //==========================================================================
    // HostConfig provider

    class HostConfigProvider : public core::platform::Provider
    {
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //==========================================================================
        // Host identification

        virtual HostInfo get_host_info() const = 0;
        virtual void set_host_name(const std::string &hostname) = 0;
        virtual void reboot() = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<HostConfigProvider> hostconfig;

    //==========================================================================
    // Signals
    extern core::signal::DataSignal<HostInfo> signal_hostinfo;
}  // namespace platform::system
