/// -*- c++ -*-
//==============================================================================
/// @file runtime.h++
/// @brief Run-time environment related functions - abstract provider
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"

namespace cc::platform
{
    enum StdFileNo
    {
        STDIN,
        STDOUT,
        STDERR
    };

    /// @brief Abstract provider for runtime related functions
    class RunTimeProvider : public Provider
    {
        using This = RunTimeProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        /// @brief Return the maximum length of a filesystem path.
        virtual bool isatty(int fd) const = 0;

        /// @brief Return the maximum length of a filesystem path.
        virtual void putenv(const std::string &envstring) = 0;
    };

    extern ProviderProxy<RunTimeProvider> runtime;
}  // namespace cc::platform
