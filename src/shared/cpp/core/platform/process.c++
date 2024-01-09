// -*- c++ -*-
//==============================================================================
/// @file process.c++
/// @brief Process invocation - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "process.h++"

namespace cc::platform
{
    PID ProcessProvider::thread_id() const
    {
        throw std::invalid_argument("thread_id() is not implemented on this platform");
    }

    PID ProcessProvider::process_id() const
    {
        throw std::invalid_argument("process_id() is not implemented on this platform");
    }

    PID ProcessProvider::invoke_async(
        const ArgVector &,
        const fs::path &,
        const fs::path &,
        const fs::path &,
        const fs::path &) const
    {
        throw std::invalid_argument("invoke_async() is not implemented on this platform");
    }

    ExitStatus ProcessProvider::invoke_sync(
        const ArgVector &,
        const fs::path &,
        const fs::path &,
        const fs::path &,
        const fs::path &) const
    {
        throw std::invalid_argument("invoke_sync() is not implemented on this platform");
    }

    PID ProcessProvider::invoke_async_pipe(
        const ArgVector &,
        const fs::path &,
        int *,
        int *,
        int *) const
    {
        throw std::invalid_argument("invoke_async_pipe() is not implemented on this platform");
    }

    ExitStatus ProcessProvider::invoke_capture(
        const ArgVector &,
        const fs::path &,
        const std::string &,
        std::string *,
        std::string *) const
    {
        throw std::invalid_argument("invoke_capture() is not implemented on this platform");
    }

    void ProcessProvider::invoke_check(
        const ArgVector &,
        const fs::path &,
        const std::string &,
        std::string *,
        std::string *) const
    {
        throw std::invalid_argument("invoke_check() is not implemented on this platform");
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<ProcessProvider> process("process");

}  // namespace cc::platform
