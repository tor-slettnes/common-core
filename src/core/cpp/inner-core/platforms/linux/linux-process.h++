// -*- c++ -*-
//==============================================================================
/// @file linux-process.h++
/// @brief Process invocation - Linux implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "posix-process.h++"

namespace core::platform
{
    /// @brief Process invocation on Linux
    class LinuxProcessProvider : public PosixProcessProvider
    {
        using This = LinuxProcessProvider;
        using Super = PosixProcessProvider;

    protected:
        LinuxProcessProvider(const std::string &name = "LinuxProcessProvider");

    public:
        std::string get_process_name_by_pid(PID pid) const override;
    };

}  // namespace core::platform
