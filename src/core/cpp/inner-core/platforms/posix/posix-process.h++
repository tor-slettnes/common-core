// -*- c++ -*-
//==============================================================================
/// @file posix-process.h++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/process.h++"

#include <poll.h>

#include <array>

namespace core::platform
{
    using Pipe = std::array<FileDescriptor, 2>;

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

        InvocationResults pipeline(
            const Invocations &invocations,
            FileDescriptor fdin,
            bool checkstatus) const override;

        InvocationResults pipe_from_stream(
            const Invocations &invocations,
            std::istream &instream,
            bool checkstatus) const override;

        ExitStatus waitpid(PID pid, bool checkstatus = false) const override;

    protected:
        PID invoke_async_from_fd(
            const ArgVector &argv,
            const fs::path &cwd,
            FileDescriptor from_fd,
            FileDescriptor *fdout,
            FileDescriptor *fderr) const;

        void write_from_stream(
            std::istream &stream,
            FileDescriptor fd) const;

        void trim_pipe(
            const Pipe &pipe,
            Direction direction,
            FileDescriptor *fd) const;

        void close_pipe(
            const Pipe &pipe) const;

        void close_fd(
            FileDescriptor fd) const;

        Pipe create_pipe() const;

        void execute(
            ArgVector argv,
            const fs::path &cwd) const;

        void poll_outputs(
            const std::vector<struct pollfd> &pfds,
            const Invocations &invocations,
            InvocationResults *results) const;

        bool check_poll(
            const struct pollfd &pfd,
            const Invocation &invocation,
            const InvocationResult &result,
            const std::string &stream_name,
            std::shared_ptr<std::stringstream> *outstream,
            std::unordered_set<FileDescriptor> *open_fds) const;

        void wait_results(
            const Invocations &invocations,
            bool checkstatus,
            InvocationResults *results) const;

    private:
        static std::unordered_set<FileDescriptor> open_fds;
    };

}  // namespace core::platform
