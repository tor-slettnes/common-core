// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-process.h++
/// @brief SysConfig native implementation - Process invocation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-process.h++"

#include <unordered_map>
#include <tuple>

namespace platform::sysconfig::native
{
    //==========================================================================
    // NativeProcessProvider

    class ProcessProvider : public ProcessInterface
    {
        using This = ProcessProvider;
        using Super = ProcessInterface;
        using FDSet = std::tuple<int, int, int>;

    protected:
        ProcessProvider();

    public:
        CommandResponse invoke_sync(const CommandInvocation &command) override;
        CommandInvocationResponse invoke_async(const CommandInvocation &command) override;
        CommandResponse invoke_finish(const CommandContinuation &input) override;

    private:
        std::unordered_map<PID, FDSet> process_map;
    };
}  // namespace platform::sysconfig::native
