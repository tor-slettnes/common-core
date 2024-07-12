/// -*- c++ -*-
//==============================================================================
/// @file linux-host.h++
/// @brief Host related functions on Linux
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-host.h++"
#include "types/valuemap.h++"

namespace core::platform
{
    /// @brief POSIX provider for host related functions
    class LinuxHostProvider : public PosixHostProvider
    {
        using This = LinuxHostProvider;
        using Super = PosixHostProvider;

    protected:
        LinuxHostProvider();

    public:
        void set_host_name(const std::string &hostname) override;

        std::string get_system_release_flavor_short() const override;
        std::string get_system_release_flavor_long() const override;
        std::string get_system_release_version_id() const override;
        std::string get_system_release_version_name() const override;
        void reboot() override;

    private:
        static types::ValueMap<std::string, std::string> read_os_release();

    private:
        types::ValueMap<std::string, std::string> os_release;
    };

}  // namespace core::platform
