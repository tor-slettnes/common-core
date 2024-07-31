/// -*- c++ -*-
//==============================================================================
/// @file posix-runtime.h++
/// @brief Run-time environment related functions - POSIX systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/runtime.h++"

namespace core::platform
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

    protected:
        /// @brief Return the maximum length of a filesystem path.
        void putenv(const std::string &envstring) override;
    };
}  // namespace core::platform
