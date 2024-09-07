// -*- c++ -*-
//==============================================================================
/// @file process.c++
/// @brief Process invocation - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "process.h++"
#include "io/streambuffer.h++"

#include <cstring>
#include <sys/wait.h>

namespace core::platform
{
    std::ostream &operator<<(std::ostream &stream, const Invocation &invocation)
    {
        stream << "{argv=" << invocation.argv
               << ", cwd=" << invocation.cwd
               << "}";

        return stream;
    }

    PID ProcessProvider::thread_id() const
    {
        throw std::invalid_argument("thread_id() is not implemented on this platform");
    }

    PID ProcessProvider::process_id() const
    {
        throw std::invalid_argument("process_id() is not implemented on this platform");
    }

    ArgVector ProcessProvider::arg_vector(const types::Value &command) const
    {
        if (const types::ValueListPtr &list = command.get_valuelist())
        {
            ArgVector argv;
            argv.reserve(list->size());
            for (types::Value &value : *list)
            {
                argv.push_back(value.as_string());
            }
            return argv;
        }
        else if (const std::string *command_string = command.get_if<std::string>())
        {
            return this->shell_command(*command_string);
        }
        else
        {
            return {};
        }
    }

    ArgVector ProcessProvider::shell_command(const std::string &command_line) const
    {
        throw std::invalid_argument("shell_command() is not implemented on this platform");
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
        FileDescriptor *,
        FileDescriptor *,
        FileDescriptor *) const
    {
        throw std::invalid_argument("invoke_async_pipe() is not implemented on this platform");
    }

    ExitStatus ProcessProvider::pipe_capture(
        PID pid,
        FileDescriptor fdin,
        FileDescriptor fdout,
        FileDescriptor fderr,
        std::istream *instream,
        std::ostream *outstream,
        std::ostream *errstream) const
    {
        throw std::invalid_argument("pipe_capture() is not implemented on this platform");
    }

    ExitStatus ProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        std::istream *instream,
        std::ostream *outstream,
        std::ostream *errstream) const
    {
        FileDescriptor fdin, fdout, fderr;
        PID pid = this->invoke_async_pipe(argv, cwd, &fdin, &fdout, &fderr);
        return this->pipe_capture(pid, fdin, fdout, fderr, instream, outstream, errstream);
    }

    ExitStatus ProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        std::istringstream stdin(input);
        std::ostringstream stdout, stderr;

        ExitStatus status = this->invoke_capture(argv, cwd, &stdin, &stdout, &stderr);
        if (output)
        {
            *output = stdout.str();
        }
        if (diag)
        {
            *diag = stderr.str();
        }
        return status;
    }

    void ProcessProvider::invoke_check(
        const ArgVector &argv,
        const fs::path &cwd,
        std::istream *instream,
        std::ostream *outstream,
        std::ostream *errstream) const
    {
        // std::ostringstream out, err;
        ExitStatus stat = this->invoke_capture(argv, cwd, instream, outstream, errstream);

        if (stat != EXIT_SUCCESS)
        {
            // std::string msg =
            //     err.tellp()   ? err.str()
            //     : out.tellp() ? out.str()
            //                   : core::str::format("[%s]: %s", stat, strerror(stat));

            // throw std::system_error(stat, std::generic_category(), msg);
            throw std::system_error(stat, std::generic_category());
        }

        // if (outstream)
        // {
        //     (*outstream) << out.rdbuf();
        // }
        // if (errstream)
        // {
        //     (*errstream) << err.rdbuf();
        // }
    }

    void ProcessProvider::invoke_check(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        std::istringstream stdin(input);
        std::ostringstream stdout, stderr;

        this->invoke_check(argv, cwd, &stdin, &stdout, &stderr);

        if (output)
        {
            *output = stdout.str();
        }
        if (diag)
        {
            *diag = stderr.str();
        }
    }

    InvocationResults ProcessProvider::pipeline(
        const Invocations &invocations,
        FileDescriptor fdin,
        bool checkstatus) const
    {
        throw std::invalid_argument("pipeline() is not implemented on this platform");
    }

    InvocationResults ProcessProvider::pipe_from_stream(
        const Invocations &invocations,
        std::istream &instream,
        bool checkstatus) const
    {
        throw std::invalid_argument("pipe_from_stream() is not implemented on this platform");
    }

    // ExitStatus ProcessProvider::waitpid(PID pid, bool checkstatus) const
    // {
    //     throw std::invalid_argument("waitpid() is not implemented on this platform");
    // }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<ProcessProvider> process("process");

}  // namespace core::platform
