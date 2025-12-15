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

namespace core::platform
{
    class PosixExitStatus : public ExitStatus
    {
    public:
        PosixExitStatus(int raw_code);

        int exit_code() const override;
        int exit_signal() const override;
        bool success() const override;
        std::string symbol() const override;
        std::string text() const override;

    private:
        int raw_code;
    };

    /// @brief Process invocation on Linux
    class PosixProcessProvider : public ProcessProvider
    {
        using This = PosixProcessProvider;
        using Super = ProcessProvider;

    protected:
        PosixProcessProvider(const std::string &name = "PosixProcessProvider");

    public:
        PID thread_id() const override;
        PID process_id() const override;

        ArgVector shell_command(const std::string &command_line) const override;

        PID fork_process() const override;

        PID invoke_async_fileio(
            const ArgVector &argv,
            const fs::path &cwd,
            const fs::path &infile,
            const fs::path &outfile,
            const fs::path &errfile,
            bool detach) const override;

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
            FileDescriptor *fderr,
            bool detach) const override;

        InvocationResult pipe_capture(
            PID pid,
            FileDescriptor fdin,
            FileDescriptor fdout,
            FileDescriptor fderr,
            std::istream *instream) const override;

        InvocationStates create_pipeline(
            const Invocations &invocations,
            FileDescriptor fdin) const override;

        InvocationResults capture_pipeline(
            const InvocationStates &states,
            bool checkstatus) const override;

        Pipe create_pipe() const override;

        FileDescriptor open_read(
            const fs::path &filename) const override;

        FileDescriptor open_write(
            const fs::path &filename,
            int create_mode) const override;

        std::size_t read_fd(
            FileDescriptor fd,
            void *buffer,
            std::size_t size) const override;

        std::size_t write_fd(
            FileDescriptor fd,
            const void *buffer,
            std::size_t length) const override;

        void close_fd(
            FileDescriptor fd) const override;

        ExitStatus::ptr waitpid(
            PID pid,
            bool checkstatus = false) const override;

    protected:
        PID invoke_async_from_fd(
            const ArgVector &argv,
            const fs::path &cwd,
            FileDescriptor from_fd,
            FileDescriptor *fdout,
            FileDescriptor *fderr,
            bool detach = false) const;

        void trim_pipe(
            const Pipe &pipe,
            PipeDirection direction,
            FileDescriptor *fd) const;

        void reassign_fd(FileDescriptor from,
                         FileDescriptor to) const;

        void close_pipe(
            const Pipe &pipe) const override;

        void close_poll(
            struct pollfd *pfd) const;

        void execute(
            ArgVector argv,
            const fs::path &cwd) const;

        void poll_outputs(
            const InvocationStates &states,
            InvocationResults *results) const;

        bool check_poll(
            const std::string &stream_name,
            struct pollfd *pfd,
            std::shared_ptr<std::stringstream> *outstream,
            std::unordered_set<FileDescriptor> *open_fds = nullptr) const;

        void wait_results(
            const InvocationStates &states,
            InvocationResults *results,
            bool checkstatus) const;

    private:
        static std::set<FileDescriptor> open_fds;
    };

}  // namespace core::platform
