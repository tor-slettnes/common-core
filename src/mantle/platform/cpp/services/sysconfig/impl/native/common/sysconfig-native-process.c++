// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-process.c++
/// @brief SysConfig native implementation - Process invocation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-native-process.h++"
#include "platform/symbols.h++"

namespace platform::sysconfig::native
{
    //==========================================================================
    // NativeProcessProvider

    ProcessProvider::ProcessProvider()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    CommandResponse ProcessProvider::invoke_sync(const CommandInvocation &command)
    {
        CommandResponse response;

        response.exit_status = core::platform::process->invoke_capture(
            command.argv,
            command.working_directory,
            command.stdin,
            &response.stdout,
            &response.stderr);
        return response;
    }

    CommandInvocationResponse ProcessProvider::invoke_async(const CommandInvocation &command)
    {
        int fdin, fdout, fderr;
        PID pid = core::platform::process->invoke_async_pipe(
            command.argv,
            command.working_directory,
            &fdin,
            &fdout,
            &fderr);

        FDSet fds{fdin, fdout, fderr};
        this->process_map.insert_or_assign(pid, fds);

        return {
            .pid = pid,
        };
    }

    CommandResponse ProcessProvider::invoke_finish(const CommandContinuation &input)
    {
        CommandResponse response;
        if (auto nh = this->process_map.extract(input.pid))
        {
            auto [fdin, fdout, fderr] = nh.mapped();
            response.exit_status = core::platform::process->pipe_capture(
                input.pid,
                fdin,
                fdout,
                fderr,
                input.stdin,
                &response.stdout,
                &response.stderr);
        }
        else
        {
            response.exit_status =
                std::make_error_code(std::errc::no_such_process).value();
        }

        return response;
    }
}  // namespace platform::sysconfig::native
