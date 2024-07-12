/// -*- c++ -*-
//==============================================================================
/// @file linux-time.h++
/// @brief Time related functions on Linux
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-time.h++"

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
        void set_time(const dt::TimePoint &tp) override;
    };

}  // namespace core::platform
