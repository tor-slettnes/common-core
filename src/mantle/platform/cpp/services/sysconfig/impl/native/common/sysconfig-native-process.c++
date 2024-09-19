// -*- c++ -*-
//==============================================================================
/// @file sysconfig-native-process.c++
/// @brief SysConfig native implementation - Process invocation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-native-process.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"

namespace platform::sysconfig::native
{
    //==========================================================================
    // NativeProcessProvider

    ProcessProvider::ProcessProvider()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    InvocationResult ProcessProvider::invoke_sync(
        const core::platform::Invocation &invocation,
        const std::string &input)
    {
        return core::platform::process->invoke_capture(
            invocation.argv,
            invocation.cwd,
            input);
    }

    PID ProcessProvider::invoke_async(
        const core::platform::Invocation &invocation,
        const std::string &input)
    {
        int fdin, fdout, fderr;
        PID pid = core::platform::process->invoke_async_pipe(
            invocation.argv,
            invocation.cwd,
            &fdin,
            &fdout,
            &fderr);

        core::platform::process->write_fd(fdin, input.data(), input.size());

        FDSet fds{fdin, fdout, fderr};
        this->process_map.insert_or_assign(pid, fds);
        return pid;
    }

    InvocationResult ProcessProvider::invoke_finish(
        PID pid,
        const std::string &input)
    {
        InvocationResult result;
        if (auto nh = this->process_map.extract(pid))
        {
            auto [fdin, fdout, fderr] = nh.mapped();
            std::stringstream stdin(input);
            return core::platform::process->pipe_capture(
                pid,
                fdin,
                fdout,
                fderr,
                &stdin);
        }
        else
        {
            throw core::exception::NotFound("No such process ID exists", pid);
        }
    }
}  // namespace platform::sysconfig::native
