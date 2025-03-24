// -*- c++ -*-
//==============================================================================
/// @file sysconfig-process.h++
/// @brief SysConfig service - Process invocation API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig-types.h++"
#include "platform/provider.h++"
#include "platform/process.h++"

namespace sysconfig
{
    using core::platform::ArgVector;
    using core::platform::ExitStatus;
    using core::platform::Invocation;
    using core::platform::InvocationResult;
    using core::platform::PID;

    //==========================================================================
    // PortableExitStatus

    class PortableExitStatus : public core::platform::ExitStatus
    {
    public:
        PortableExitStatus(bool success,
                           int exit_code,
                           int exit_signal,
                           const std::string &symbol,
                           const std::string &text);

        int exit_code() const override;
        int exit_signal() const override;
        bool success() const override;
        std::string symbol() const override;
        std::string text() const override;

    private:
        bool success_;
        int code_;
        int signal_;
        std::string symbol_;
        std::string text_;
    };

    //==========================================================================
    // ProcessInterface

    class ProcessInterface : public core::platform::Provider
    {
        using This = ProcessInterface;
        using Super = core::platform::Provider;

    protected:
        using Super::Super;

    public:
        //==========================================================================
        // Spawn a new process, with or without capturing stdin/stdout/stderr.

        virtual InvocationResult invoke_sync(
            const Invocation &invocation,
            const std::string &input) = 0;

        virtual PID invoke_async(
            const Invocation &invocation,
            const std::string &input) = 0;

        virtual InvocationResult invoke_finish(
            PID pid,
            const std::string &input) = 0;
    };

    //==========================================================================
    // Process provider instance

    extern core::platform::ProviderProxy<ProcessInterface> process;
}  // namespace sysconfig
