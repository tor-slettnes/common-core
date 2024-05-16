/// -*- c++ -*-
//==============================================================================
/// @file windows-runtime.h++
/// @brief Run-time environment related functions - WINDOWS systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/runtime.h++"

namespace cc::platform
{
    /// @brief Abstract provider for runtime related functions
    class WindowsRunTimeProvider : public RunTimeProvider
    {
        using This = WindowsRunTimeProvider;
        using Super = RunTimeProvider;

    protected:
        WindowsRunTimeProvider();

    public:
        /// @brief Return the maximum length of a filesystem path.
        bool isatty(int fd) const override;

        /// @brief Return the maximum length of a filesystem path.
        void putenv(const std::string &envstring) override;
    };
}  // namespace cc::platform
