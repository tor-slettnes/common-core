/// -*- c++ -*-
//==============================================================================
/// @file linux-time.h++
/// @brief Time related functions on Linux
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-time.h++"
#include "types/valuemap.h++"

namespace core::platform
{
    /// @brief POSIX provider for host related functions
    class LinuxTimeProvider : public PosixTimeProvider
    {
        using This = LinuxTimeProvider;
        using Super = PosixTimeProvider;

    protected:
        LinuxTimeProvider();

    public:
        bool is_pertinent() const override;
        void set_time(const dt::TimePoint &tp) override;

        void set_ntp(bool ntp) override;
        bool get_ntp() const override;

        void set_ntp_servers(const std::vector<std::string> &servers) override;
        std::vector<std::string> get_ntp_servers() const override;

    private:
        core::types::ValueMap<std::string, std::string> read_settings() const;
    };

}  // namespace core::platform
