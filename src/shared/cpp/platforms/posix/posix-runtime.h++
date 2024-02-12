/// -*- c++ -*-
//==============================================================================
/// @file posix-runtime.h++
/// @brief Run-time environment related functions - POSIX systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/runtime.h++"

namespace shared::platform
{
    /// @brief Abstract provider for runtime related functions
    class PosixRunTimeProvider : public RunTimeProvider
    {
        using This = PosixRunTimeProvider;
        using Super = RunTimeProvider;

    protected:
        PosixRunTimeProvider();

    public:
        /// @brief Return the maximum length of a filesystem path.
        bool isatty(int fd) const override;

        /// @brief Return the maximum length of a filesystem path.
        void putenv(const std::string &envstring) override;
    };
}  // namespace shared::platform
