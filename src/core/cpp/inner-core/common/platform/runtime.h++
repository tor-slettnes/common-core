/// -*- c++ -*-
//==============================================================================
/// @file runtime.h++
/// @brief Run-time environment related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "provider.h++"
#include "types/valuemap.h++"

namespace core::platform
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
        virtual std::optional<std::string> getenv(const std::string &variable);

        // @brief Add a new environment variable.  This effectively wraps
        //     OS-specific `putenv()` implementations by ensuring the newly
        //     added environment string remains in memory (something which
        //     `putenv()` does not do).
        void setenv(const std::string &variable, const std::string &value);

        // @brief remove an exiting environment variable.  If this variable
        //     had been previously added with `setenv()` it is also removed
        //     from our local cache (thus freed).
        void unsetenv(const std::string &variable);

        /// @brief Return the maximum length of a filesystem path.
        virtual bool isatty(int fd) const = 0;

    protected:
        /// @brief Add a an entry of the form "VARIABLE=value" to the environment.
        virtual void putenv(const std::string &envstring) = 0;

    private:
        static types::ValueMap<std::string, std::string> env;
    };

    extern ProviderProxy<RunTimeProvider> runtime;
}  // namespace core::platform
