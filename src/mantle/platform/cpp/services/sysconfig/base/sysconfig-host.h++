// -*- c++ -*-
//==============================================================================
/// @file sysconfig-host.h++
/// @brief SysConfig service - Host Configuration API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "thread/signaltemplate.h++"

namespace platform::sysconfig
{
    //==========================================================================
    // HostInfo

    struct HostInfo
    {
        std::string hostname;

        std::string os_name;
        std::string os_version;
        std::string os_release_flavor_short;
        std::string os_release_flavor_long;
        std::string os_release_version_id;
        std::string os_release_version_name;

        std::string machine_arch;
        std::string machine_cpu;
        std::string machine_name;
        std::string machine_description;
        std::string machine_serial;
        std::string machine_uid;
    };

    std::ostream &operator<<(std::ostream &stream, const HostInfo &hi);

    //==========================================================================
    // HostConfig provider

    class HostConfigInterface : public core::platform::Provider
    {
        using This = HostConfigInterface;
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

    extern core::platform::ProviderProxy<HostConfigInterface> hostconfig;

    //==========================================================================
    // Signals
    extern core::signal::DataSignal<HostInfo> signal_hostinfo;
}  // namespace platform::sysconfig
