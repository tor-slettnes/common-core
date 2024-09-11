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
        InvocationResult invoke_sync(
            const Invocation &invocation,
            const std::string &input) override;

        PID invoke_async(
            const Invocation &invocation,
            const std::string &input) override;

        InvocationResult invoke_finish(
            PID pid,
            const std::string &input) override;

    private:
        std::unordered_map<PID, FDSet> process_map;
    };
}  // namespace platform::sysconfig::native
