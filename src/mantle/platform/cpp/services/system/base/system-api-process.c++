// -*- c++ -*-
//==============================================================================
/// @file system-api-process.c++
/// @brief System service - Process invocation API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-api-process.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

#include <system_error>

namespace platform::system
{
    CommandResponse ProcessProvider::invoke_sync(const CommandInvocation &command)
    {
        core::platform::ArgVector argv(command.argv().begin(), command.argv().end());
        CommandResponse response;

        response.set_exit_status(core::platform::process->invoke_capture(
            argv,
            command.working_directory(),
            command.stdin(),
            response.mutable_stdout(),
            response.mutable_stderr()));
        return response;
    }

    CommandInvocationStatus ProcessProvider::invoke_async(const CommandInvocation &command)
    {
        core::platform::ArgVector argv(command.argv().begin(), command.argv().end());
        int fdin, fdout, fderr;
        PID pid = core::platform::process->invoke_async_pipe(
            argv,
            command.working_directory(),
            &fdin,
            &fdout,
            &fderr);

        FDSet fds{fdin, fdout, fderr};
        this->process_map.insert_or_assign(pid, fds);

        CommandInvocationStatus status;
        status.set_pid(pid);
        return status;
    }

    CommandResponse ProcessProvider::invoke_finish(const CommandInput &input)
    {
        CommandResponse response;
        if (auto nh = this->process_map.extract(input.pid()))
        {
            auto [fdin, fdout, fderr] = nh.mapped();
            response.set_exit_status(core::platform::process->pipe_capture(
                input.pid(),
                fdin,
                fdout,
                fderr,
                input.stdin(),
                response.mutable_stdout(),
                response.mutable_stderr()));
        }
        else
        {
            response.set_exit_status(
                std::make_error_code(std::errc::no_such_process).value());
        }
        return response;
    }

    core::platform::ProviderProxy<ProcessProvider> process("process");
}  // namespace platform::system
