// -*- c++ -*-
//==============================================================================
/// @file process.c++
/// @brief Process invocation - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "process.h++"
#include "io/streambuffer.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <sys/wait.h>

#include <cstring>
#include <future>
#include <utility>

namespace core::platform
{
    // std::ostream &operator<<(std::ostream &stream, const Invocation &invocation)
    // {
    //     stream << "{argv=" << invocation.argv
    //            << ", cwd=" << invocation.cwd
    //            << "}";

    //     return stream;
    // }

    //==========================================================================
    // ExitStatus

    int ExitStatus::combined_code() const
    {
        if (int code = this->exit_code())
        {
            return code;
        }
        else
        {
            return this->exit_signal();
        }
    }

    void ExitStatus::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add("code", this->exit_code(), this->exit_code() != 0);
        parts.add("signal", this->exit_signal(), this->exit_signal() != 0);
        parts.add("symbol", this->symbol(), !this->symbol().empty(), "%r");
        parts.add("text", this->text(), !this->text().empty(), "%r");
        stream << parts;
    }

    //==========================================================================
    // InvocationResult

    InvocationResult::InvocationResult(
        PID pid,
        ExitStatus::ptr status,
        std::shared_ptr<std::stringstream> stdout,
        std::shared_ptr<std::stringstream> stderr)
        : pid(pid),
          status(status),
          stdout(stdout ? stdout : std::make_shared<std::stringstream>()),
          stderr(stderr ? stderr : std::make_shared<std::stringstream>())
    {
    }

    int InvocationResult::error_code() const
    {
        if (this->status)
        {
            return this->status->combined_code();
        }
        else
        {
            return 0;
        }
    }

    std::string InvocationResult::error_symbol() const
    {
        if (this->status)
        {
            return this->status->symbol();
        }
        else
        {
            return "OK";
        }
    }

    std::string InvocationResult::error_text() const
    {
        if (this->error_code())
        {
            if (this->stderr && (this->stderr->tellp() > 0))
            {
                return this->stderr->str();
            }
            else if (this->stdout && (this->stdout->tellp() > 0))
            {
                return this->stdout->str();
            }
            else
            {
                return this->status->text();
            }
        }
        else
        {
            return {};
        }
    }

    std::string InvocationResult::stdout_text() const
    {
        if (this->stdout)
        {
            return this->stdout->str();
        }
        else
        {
            return {};
        }
    }

    std::string InvocationResult::stderr_text() const
    {
        if (this->stderr)
        {
            return this->stderr->str();
        }
        else
        {
            return {};
        }
    }

    std::ios::pos_type InvocationResult::stdout_size() const
    {
        if (this->stdout)
        {
            return this->stdout->tellp();
        }
        else
        {
            return 0;
        }
    }

    std::ios::pos_type InvocationResult::stderr_size() const
    {
        if (this->stderr)
        {
            return this->stderr->tellp();
        }
        else
        {
            return 0;
        }
    }

    void InvocationResult::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        if (this->status)
        {
            parts.add("status", *this->status);
        }
        else
        {
            parts.add("status", "success");
        }

        parts.add("stdout", this->stdout_text(), this->stdout_size(), "%r");
        parts.add("stderr", this->stderr_text(), this->stderr_size(), "%r");
        stream << parts;
    }

    //==========================================================================
    // ProcessProvider

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

    PID ProcessProvider::invoke_async_fileio(
        const ArgVector &,
        const fs::path &,
        const fs::path &,
        const fs::path &,
        const fs::path &) const
    {
        throw std::invalid_argument("invoke_async() is not implemented on this platform");
    }

    ExitStatus::ptr ProcessProvider::invoke_sync_fileio(
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

    InvocationResult ProcessProvider::pipe_capture(
        PID pid,
        FileDescriptor fdin,
        FileDescriptor fdout,
        FileDescriptor fderr,
        std::istream *instream) const
    {
        throw std::invalid_argument("pipe_capture() is not implemented on this platform");
    }

    InvocationResult ProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        std::istream *instream) const
    {
        FileDescriptor fdin, fdout, fderr;
        PID pid = this->invoke_async_pipe(argv, cwd, &fdin, &fdout, &fderr);
        return this->pipe_capture(pid, fdin, fdout, fderr, instream);
    }

    InvocationResult ProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input) const
    {
        std::istringstream stdin(input);
        return this->invoke_capture(argv, cwd, &stdin);
    }

    void ProcessProvider::invoke_check(
        const ArgVector &argv,
        const fs::path &cwd,
        std::istream *instream) const
    {
        // std::ostringstream out, err;
        InvocationResult result = this->invoke_capture(argv, cwd, instream);

        if (!result.status->success())
        {
            throw core::exception::InvocationError(
                argv.front(),
                result.status);
        }
    }

    void ProcessProvider::invoke_check(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input) const
    {
        std::istringstream stdin(input);
        this->invoke_check(argv, cwd, &stdin);
    }

    InvocationResults ProcessProvider::pipe_from_stream(
        const Invocations &invocations,
        std::istream &instream,
        bool checkstatus) const
    {
        Pipe inpipe = this->create_pipe();
        logf_trace("Created pipe from stream, %d -> %d", inpipe[OUTPUT], inpipe[INPUT]);

        std::future<InvocationResults> future = std::async(
            &This::pipeline, this, invocations, inpipe[INPUT], checkstatus);

        this->write_from_stream(&instream, inpipe[OUTPUT]);
        this->close_fd(inpipe[OUTPUT]);
        return future.get();
    }

    InvocationResults ProcessProvider::pipeline(
        const Invocations &invocations,
        FileDescriptor fdin,
        bool checkstatus) const
    {
        InvocationStates states = this->create_pipeline(invocations, fdin);
        return this->capture_pipeline(states, checkstatus);
    }

    void ProcessProvider::write_from_stream(
        std::istream *stream,
        FileDescriptor fd) const
    {
        std::vector<char> buffer(CHUNKSIZE);
        std::size_t count = 0;
        while (stream->good())
        {
            stream->read(reinterpret_cast<char *>(buffer.data()), buffer.size());
            count += stream->gcount();
            this->write_fd(fd, buffer.data(), stream->gcount());
        }
    }

    /// Global instance, populated with the "best" provider for this system.
    ProviderProxy<ProcessProvider> process("process");

}  // namespace core::platform
