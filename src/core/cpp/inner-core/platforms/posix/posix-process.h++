// -*- c++ -*-
//==============================================================================
/// @file posix-process.h++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/process.h++"

namespace core::platform
{
    /// @brief Process invocation on Linux
    class PosixProcessProvider : public ProcessProvider
    {
        using This = PosixProcessProvider;
        using Super = ProcessProvider;

    protected:
        enum Direction
        {
            INPUT = 0,
            OUTPUT = 1
        };

        // struct InvocationStatus
        // {
        //     std::string command;
        //     PID pid;
        //     FileDescriptor fd_out;
        //     FileDescriptor fd_err;
        //     std::exception_ptr eptr;

        // };

    protected:
        PosixProcessProvider(const std::string &name = "PosixProcessProvider");

    public:
        PID thread_id() const override;
        PID process_id() const override;

        ArgVector shell_command(const std::string &command_line) const override;

        PID invoke_async(
            const ArgVector &argv,
            const fs::path &cwd,
            const fs::path &infile,
            const fs::path &outfile,
            const fs::path &errfile) const override;

        ExitStatus invoke_sync(
            const ArgVector &argv,
            const fs::path &cwd,
            const fs::path &infile,
            const fs::path &outfile,
            const fs::path &errfile) const override;

        PID invoke_async_pipe(
            const ArgVector &argv,
            const fs::path &cwd,
            FileDescriptor *fdin,
            FileDescriptor *fdout,
            FileDescriptor *fderr) const override;

        PID invoke_async_from_pipe(
            const ArgVector &argv,
            const fs::path &cwd,
            FileDescriptor from_fdin,
            FileDescriptor *fdout,
            FileDescriptor *fderr) const override;

        ExitStatus pipe_capture(
            PID pid,
            FileDescriptor fdin,
            FileDescriptor fdout,
            FileDescriptor fderr,
            const std::string &input,
            std::string *output,
            std::string *diag) const override;

        ExitStatus invoke_capture(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input,
            std::string *output,
            std::string *diag) const override;

        void invoke_check(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input,
            std::string *output,
            std::string *diag) const override;

        void pipeline(
            const Invocations &invocations,
            FileDescriptor fdin,
            FileDescriptor *fdout) const override;

        void pipe_from_stream(
            const Invocations &invocations,
            std::istream &instream,
            FileDescriptor *fdout) const override;

        ExitStatus waitpid(PID pid, bool checkstatus = false) const override;

    protected:
        void write_from_stream(
            std::istream &stream,
            FileDescriptor fd) const;

        void trim_pipe(
            FileDescriptor pipefd[2],
            Direction direction,
            FileDescriptor *fd) const;

        void close_fds(
            FileDescriptor pipefd[2]) const;

        void execute(
            ArgVector argv,
            const fs::path &cwd) const;
    };

}  // namespace core::platform
