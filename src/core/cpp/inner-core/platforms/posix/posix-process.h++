// -*- c++ -*-
//==============================================================================
/// @file posix-process.h++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/process.h++"
#include "types/create-shared.h++"

#include <poll.h>

#include <array>

namespace core::platform
{
    using Pipe = std::array<FileDescriptor, 2>;

    class PosixExitStatus : public ExitStatus
    {
    public:
        PosixExitStatus(int combined_code);

        int exit_code() const override;
        int exit_signal() const override;
        bool success() const override;
        std::string symbol() const override;
        std::string text() const override;

    private:
        int code_;
    };


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

    protected:
        PosixProcessProvider(const std::string &name = "PosixProcessProvider");

    public:
        PID thread_id() const override;
        PID process_id() const override;

        ArgVector shell_command(const std::string &command_line) const override;

        PID invoke_async_fileio(
            const ArgVector &argv,
            const fs::path &cwd,
            const fs::path &infile,
            const fs::path &outfile,
            const fs::path &errfile) const override;

        ExitStatus::ptr invoke_sync_fileio(
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

        InvocationResult pipe_capture(
            PID pid,
            FileDescriptor fdin,
            FileDescriptor fdout,
            FileDescriptor fderr,
            std::istream *instream) const override;

        InvocationResults pipeline(
            const Invocations &invocations,
            FileDescriptor fdin,
            bool checkstatus) const override;

        InvocationResults pipe_from_stream(
            const Invocations &invocations,
            std::istream &instream,
            bool checkstatus) const override;

        std::size_t readfd(
            FileDescriptor fd,
            void *buffer,
            std::size_t bufsize) const override;

        std::size_t writefd(
            FileDescriptor fd,
            const void *buffer,
            std::size_t length) const override;

        ExitStatus::ptr waitpid(
            PID pid,
            bool checkstatus = false) const override;

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
