/// -*- c++ -*-
//==============================================================================
/// @file runtime.c++
/// @brief Run-time environment related functions - abstract provider
/// @author Tor Slettnes
//==============================================================================

#include "runtime.h++"

#include <cstdlib>

namespace core::platform
{
    std::optional<std::string> RunTimeProvider::getenv(const std::string &variable)
    {
        if (char *value = std::getenv(variable.data()))
        {
            return value;
        }
        else
        {
            return {};
        }
    }

    void RunTimeProvider::setenv(
        const std::string &variable,
        const std::string &value)
    {
        std::string &envstring = This::env[variable] = variable + "=" + value;
        this->putenv(envstring);
    }

    void RunTimeProvider::unsetenv(
        const std::string &variable)
    {
        this->putenv(variable);
        This::env.erase(variable);
    }

    types::ValueMap<std::string, std::string> RunTimeProvider::env;

    ProviderProxy<RunTimeProvider> runtime("runtime environment");
}  // namespace core::platform
