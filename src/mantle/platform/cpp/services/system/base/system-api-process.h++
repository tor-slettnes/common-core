// -*- c++ -*-
//==============================================================================
/// @file system-api-process.h++
/// @brief System service - Process invocation API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-types.h++"
#include "platform/provider.h++"
#include "platform/process.h++"

#include <unordered_map>
#include <tuple>

namespace platform::system
{
    using core::platform::ExitStatus;
    using core::platform::PID;
    using core::platform::ArgVector;

    using ChildWatcher = std::function<void(PID pid, ExitStatus status)>;
    using FDSet = std::tuple<int, int, int>;

    class ProcessProvider : public core::platform::Provider
    {
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //==========================================================================
        // Spawn a new process, with or without capturing stdin/stdout/stderr.

        CommandResponse invoke_sync(const CommandInvocation &command);
        CommandInvocationStatus invoke_async(const CommandInvocation &command);
        CommandResponse invoke_finish(const CommandInput &input);

    private:
        std::unordered_map<PID, FDSet> process_map;
    };

    extern core::platform::ProviderProxy<ProcessProvider> process;
}  // namespace platform
