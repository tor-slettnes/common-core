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

namespace platform::sysconfig
{
    using core::platform::ArgVector;
    using core::platform::ExitStatus;
    using core::platform::PID;

    //==========================================================================
    // CommandInvocation

    struct CommandInvocation
    {
        std::filesystem::path working_directory;
        std::vector<std::string> argv;
        std::string stdin;
    };

    //==========================================================================
    // CommandInvocation

    struct CommandInvocationResponse
    {
        PID pid = 0;
    };

    //==========================================================================
    // CommandContinuation

    struct CommandContinuation
    {
        PID pid = 0;
        std::string stdin;
    };

    //==========================================================================
    // CommandResponse

    struct CommandResponse
    {
        std::string stdout;
        std::string stderr;
        int exit_status = 0;
    };

    //==========================================================================
    // CommandPipe

    struct CommandPipe
    {
        PID pid;
        int stdin;
        int stdout;
        int stderr;
    };

    //==========================================================================
    // Stream I/O

    // std::ostream operator<<(std::ostream &stream, const CommandInvocation &invocation);
    // std::ostream operator<<(std::ostream &stream, const CommandInvocationResponse &response);
    // std::ostream operator<<(std::ostream &stream, const CommandContinuation &continuation);
    // std::ostream operator<<(std::ostream &stream, const CommandResponse &response);

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

        virtual CommandResponse invoke_sync(const CommandInvocation &command) = 0;
        virtual CommandInvocationResponse invoke_async(const CommandInvocation &command) = 0;
        virtual CommandResponse invoke_finish(const CommandContinuation &input) = 0;
    };

    //==========================================================================
    // Process provider instance

    extern core::platform::ProviderProxy<ProcessInterface> process;
}  // namespace platform::sysconfig
