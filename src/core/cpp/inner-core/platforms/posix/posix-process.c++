// -*- c++ -*-
//==============================================================================
/// @file posix-process.c++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-process.h++"

#include <stdio.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define CHUNKSIZE       4096
#define ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

namespace shared::platform
{
    PosixProcessProvider::PosixProcessProvider(const std::string &name)
        : Super(name)
    {
    }

    PID PosixProcessProvider::thread_id() const
    {
        return gettid();
    }

    PID PosixProcessProvider::process_id() const
    {
        return getpid();
    }

    PID PosixProcessProvider::invoke_async(
        const ArgVector &argv,
        const fs::path &cwd,
        const fs::path &infile,
        const fs::path &outfile,
        const fs::path &errfile) const
    {
        if (argv.empty())
        {
            throw std::invalid_argument("Missing command");
        }

        PID pid = this->checkstatus(fork());

        if (pid == 0)
        {
            // Child closes stdin, stdout, stderr, then invokes command
            if (!infile.empty())
            {
                this->checkstatus(freopen(infile.c_str(), "r", stdin));
            }

            if (!outfile.empty())
            {
                this->checkstatus(freopen(outfile.c_str(), "w", stdout));
            }

            if (!errfile.empty())
            {
                this->checkstatus(freopen(errfile.c_str(), "w", stderr));
            }

            this->execute(argv, cwd);
        }

        return pid;
    }

    ExitStatus PosixProcessProvider::invoke_sync(
        const ArgVector &argv,
        const fs::path &cwd,
        const fs::path &infile,
        const fs::path &outfile,
        const fs::path &errfile) const
    {
        PID pid = this->invoke_async(argv, cwd, infile, outfile, errfile);
        int wstatus = 0;
        int options = 0;
        waitpid(pid, &wstatus, options);
        return wstatus;
    }

    PID PosixProcessProvider::invoke_async_pipe(
        const ArgVector &argv,
        const fs::path &cwd,
        int *fdin,
        int *fdout,
        int *fderr) const
    {
        if (argv.empty())
        {
            throw std::invalid_argument("Missing command");
        }

        int inpipe[2], outpipe[2], errpipe[2];

        this->checkstatus(pipe(inpipe));
        this->checkstatus(pipe(outpipe));
        this->checkstatus(pipe(errpipe));

        PID pid = this->checkstatus(fork());

        if (pid == 0)
        {
            // Child uses inpipe/outpipe/errpipe as stdin/stdout/sderr.
            this->checkstatus(dup2(inpipe[INPUT], STDIN_FILENO));
            this->checkstatus(dup2(outpipe[OUTPUT], STDOUT_FILENO));
            this->checkstatus(dup2(errpipe[OUTPUT], STDERR_FILENO));

            this->close_fds(inpipe);
            this->close_fds(outpipe);
            this->close_fds(errpipe);

            this->execute(argv, cwd);
        }
        else
        {
            // Parent writes to inpipe and reads from outpipe and errpipe.
            this->trim_pipe(inpipe, OUTPUT, fdin);
            this->trim_pipe(outpipe, INPUT, fdout);
            this->trim_pipe(errpipe, INPUT, fderr);
        }

        return pid;
    }

    ExitStatus PosixProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        int fdin, fdout, fderr;
        PID pid = this->invoke_async_pipe(argv, cwd, &fdin, &fdout, &fderr);

        struct pollfd pfds[] = {
            {fdin, POLLOUT, 0},
            {fdout, POLLIN, 0},
            {fderr, POLLIN, 0}};

        for (uint i = 0; i < ELEMENTS(pfds); i++)
        {
            fcntl(pfds[i].fd, F_SETFL, O_NONBLOCK);
        }

        std::string::size_type written = 0;
        char outbuf[CHUNKSIZE];
        std::stringstream out, err;
        ExitStatus exitstatus;

        while (waitpid(pid, &exitstatus, WNOHANG) == 0)
        {
            if (written >= input.size())
            {
                close(fdin);
                pfds[STDIN_FILENO].fd = -1;
                pfds[STDIN_FILENO].events = 0;
            }

            this->checkstatus(poll(pfds, ELEMENTS(pfds), -1));

            if (pfds[STDIN_FILENO].revents & POLLOUT)
            {
                const char *inbuf = input.data() + written;
                if (ssize_t bytes = this->checkstatus(write(fdin, inbuf, input.size() - written)))
                {
                    written += bytes;
                }
            }

            if (pfds[STDOUT_FILENO].revents & POLLIN)
            {
                int nchars = this->checkstatus(read(fdout, (void *)outbuf, CHUNKSIZE));
                if (nchars && output)
                {
                    out.write(outbuf, nchars);
                }
            }

            if (pfds[STDERR_FILENO].revents & POLLIN)
            {
                int nchars = this->checkstatus(read(fderr, (void *)outbuf, CHUNKSIZE));
                if (nchars && diag)
                {
                    err.write(outbuf, nchars);
                }
            }
        }

        for (uint i = 0; i < ELEMENTS(pfds); i++)
        {
            if (pfds[i].fd != -1)
            {
                close(pfds[i].fd);
            }
        }

        if (output)
        {
            *output = out.str();
        }

        if (diag)
        {
            *diag = err.str();
        }

        return exitstatus;
    }

    void PosixProcessProvider::invoke_check(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        std::string out, err;

        ExitStatus stat = this->invoke_capture(argv, cwd, input, &out, &err);
        if (output)
        {
            *output = out;
        }
        if (diag)
        {
            *diag = err;
        }

        if (stat != 0)
        {
            if (err.empty())
            {
                err = std::move(out);
            }
            if (err.empty())
            {
                err = "[" + std::to_string(stat) + "]: " + strerror(stat);
            }
            throw std::system_error(stat, std::generic_category(), err);
        }
    }

    void PosixProcessProvider::trim_pipe(int pipefd[2], Direction direction, int *fd) const
    {
        close(pipefd[!direction]);
        if (fd)
        {
            *fd = pipefd[direction];
        }
        else
        {
            close(pipefd[direction]);
        }
    }

    void PosixProcessProvider::close_fds(int pipefd[2]) const
    {
        close(pipefd[INPUT]);
        close(pipefd[OUTPUT]);
    }

    void PosixProcessProvider::execute(ArgVector argv, const fs::path &cwd) const
    {
        if (!cwd.empty())
        {
            this->checkstatus(chdir(cwd.string().data()));
        }

        // Convert our argument vector into a plain old
        // null-terminated array of null-terminated character arrays
        std::vector<char *> c_argv(argv.size() + 1);
        char **c_argp = c_argv.data();
        for (std::string &arg : argv)
        {
            *(c_argp++) = arg.data();
        }
        *c_argp = nullptr;

        // Here we go. This should be the last thing we do. Goodbye!
        execv(c_argv.at(0), c_argv.data());

        // Why are we still here?
        throw std::system_error(errno, std::system_category());
    }

}  // namespace shared::platform
