/// -*- c++ -*-
//==============================================================================
/// @file posix-host.h++
/// @brief Host related functions on POSIX compatible hosts
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "platform/host.h++"

namespace core::platform
{
    /// @brief POSIX provider for host related functions
    class PosixHostProvider : public HostProvider
    {
        using This = PosixHostProvider;
        using Super = HostProvider;

    protected:
        using Super::Super;

    public:
        std::string get_host_name() const override;
        void set_host_name(const std::string &hostname) override;

        std::string get_system_name() const override;
        std::string get_system_version() const override;

        void reboot() override;
    };

}  // namespace core::platform
