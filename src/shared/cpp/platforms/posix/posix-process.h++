// -*- c++ -*-
//==============================================================================
/// @file posix-process.h++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/process.h++"

namespace shared::platform
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

    protected:
        PosixProcessProvider(const std::string &name = "PosixProcessProvider");

    public:
        PID thread_id() const override;
        PID process_id() const override;

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
            int *fdin,
            int *fdout,
            int *fderr) const override;

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

    protected:
        void trim_pipe(int pipefd[2], Direction direction, int *fd) const;
        void close_fds(int pipefd[2]) const;
        void execute(ArgVector argv, const fs::path &cwd) const;
    };

}  // namespace shared::platform
