// -*- c++ -*-
//==============================================================================
/// @file posix-process.c++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-process.h++"

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <future>


#define ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

namespace core::platform
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

    ArgVector PosixProcessProvider::shell_command(const std::string &command_line) const
    {
        return {"/bin/sh", "-c", command_line};
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
        return this->waitpid(pid, false);
    }

    PID PosixProcessProvider::invoke_async_pipe(
        const ArgVector &argv,
        const fs::path &cwd,
        FileDescriptor *fdin,
        FileDescriptor *fdout,
        FileDescriptor *fderr) const
    {
        if (argv.empty())
        {
            throw std::invalid_argument("Missing command");
        }

        FileDescriptor inpipe[2], outpipe[2], errpipe[2];

        this->checkstatus(::pipe(inpipe));
        this->checkstatus(::pipe(outpipe));
        this->checkstatus(::pipe(errpipe));

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

    PID PosixProcessProvider::invoke_async_from_pipe(
        const ArgVector &argv,
        const fs::path &cwd,
        FileDescriptor from_fdin,
        FileDescriptor *fdout,
        FileDescriptor *fderr) const
    {
        if (argv.empty())
        {
            throw std::invalid_argument("Missing command");
        }

        FileDescriptor outpipe[2], errpipe[2];

        this->checkstatus(::pipe(outpipe));
        this->checkstatus(::pipe(errpipe));

        PID pid = this->checkstatus(fork());

        if (pid == 0)
        {
            // Child uses from_fdin as input and outpipe/errpipe as stdout/sderr.
            this->checkstatus(dup2(from_fdin, STDIN_FILENO));
            this->checkstatus(dup2(outpipe[OUTPUT], STDOUT_FILENO));
            this->checkstatus(dup2(errpipe[OUTPUT], STDERR_FILENO));

            close(from_fdin);
            this->close_fds(outpipe);
            this->close_fds(errpipe);

            this->execute(argv, cwd);
        }
        else
        {
            // Parent reads from outpipe and errpipe.
            close(from_fdin);
            this->trim_pipe(outpipe, INPUT, fdout);
            this->trim_pipe(errpipe, INPUT, fderr);
        }

        return pid;
    }

    ExitStatus PosixProcessProvider::pipe_capture(
        PID pid,
        FileDescriptor fdin,
        FileDescriptor fdout,
        FileDescriptor fderr,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        struct pollfd pfds[] = {
            {fdin, POLLOUT, 0},
            {fdout, POLLIN, 0},
            {fderr, POLLIN, 0}};

        for (uint i = 0; i < ELEMENTS(pfds); i++)
        {
            if (FileDescriptor fd = pfds[i].fd; fd >= 0)
            {
                fcntl(fd, F_SETFL, O_NONBLOCK);
            }
        }

        std::string::size_type written = 0;
        std::vector<char> outbuf(CHUNKSIZE);
        std::stringstream out, err;
        ExitStatus exitstatus;

        while (::waitpid(pid, &exitstatus, WNOHANG) == 0)
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
                int nchars = this->checkstatus(read(fdout, (void *)outbuf.data(), outbuf.size()));
                if (nchars && output)
                {
                    out.write(outbuf.data(), nchars);
                }
            }

            if (pfds[STDERR_FILENO].revents & POLLIN)
            {
                int nchars = this->checkstatus(read(fderr, (void *)outbuf.data(), outbuf.size()));
                if (nchars && diag)
                {
                    err.write(outbuf.data(), nchars);
                }
            }
        }

        for (uint i = 0; i < ELEMENTS(pfds); i++)
        {
            if (FileDescriptor fd = pfds[i].fd; fd >= 0)
            {
                close(fd);
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

    ExitStatus PosixProcessProvider::invoke_capture(
        const ArgVector &argv,
        const fs::path &cwd,
        const std::string &input,
        std::string *output,
        std::string *diag) const
    {
        FileDescriptor fdin, fdout, fderr;
        PID pid = this->invoke_async_pipe(argv, cwd, &fdin, &fdout, &fderr);
        return this->pipe_capture(pid, fdin, fdout, fderr, input, output, diag);
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

    void PosixProcessProvider::pipeline(
        const Invocations &invocations,
        FileDescriptor fdin,
        FileDescriptor *fdout) const
    {
        FileDescriptor pipe_fd = fdin;
        std::vector<PID> pids;
        std::vector<struct pollfd> stderr_pfds;
        std::vector<std::shared_ptr<std::stringstream>> stderr_streams(invocations.size());

        // std::vector<std::pair<PID, std::string>> processes;
        // processes.reserve(invocations.size());
        // std::vector<std::tuple<std::string, PID, FileDescriptor>> results;
        // results.reserve(invocations.size());

        // Invoke commands
        for (const Invocation &invocation : invocations)
        {
            FileDescriptor stderr_fd;
            pids.push_back(
                this->invoke_async_from_pipe(
                    invocation.argv,
                    invocation.cwd,
                    pipe_fd,
                    &pipe_fd,
                    &stderr_fd));

            stderr_pfds.push_back({
                .fd = stderr_fd,
                .events = POLLIN,
                .revents = 0,
            });

            fcntl(stderr_fd, F_SETFL, O_NONBLOCK);
        }

        if (fdout != nullptr)
        {
            *fdout = pipe_fd;
        }

        // Collect anything printed on `stderr` from each subprocess.
        // We do this with `poll()` rather than sequentially in order to
        // avoid deadlocks in case the text from a given process fills
        // up the corresponding pipe's temporary buffer.

        std::vector<char> outbuf(CHUNKSIZE);
        for (bool received = true; received; received = false)
        {
            this->checkstatus(poll(stderr_pfds.data(), stderr_pfds.size(), -1));

            for (uint c = 0; c < stderr_pfds.size(); c++)
            {
                struct pollfd &pfd = stderr_pfds.at(c);
                if (pfd.revents & POLLIN)
                {
                    std::shared_ptr<std::stringstream> &ss = stderr_streams.at(c);
                    if (!ss)
                    {
                        ss = std::make_shared<std::stringstream>();
                        *ss << invocations.at(c).argv.front() << ": ";
                    }

                    int nchars = this->checkstatus(
                        ::read(pfd.fd,
                               (void *)outbuf.data(),
                               outbuf.size()));

                    ss->write(outbuf.data(), nchars);
                    received = true;
                }
            }
        }

        // Now wait for all subprocesses to complete.  We capture the error from
        // the first process to exit with a non-zero code, if any.
        std::exception_ptr eptr;
        for (uint c = 0; c < pids.size(); c++)
        {
            if (ExitStatus err = this->waitpid(pids.at(c)))
            {
                if (!eptr)
                {
                    eptr = std::make_exception_ptr(
                        std::system_error(err,
                                          std::system_category(),
                                          invocations.at(c).argv.front()));
                }
            }
        }

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

    void PosixProcessProvider::pipe_from_stream(
        const Invocations &invocations,
        std::istream &instream,
        FileDescriptor *fdout) const
    {
        FileDescriptor inpipe[2];
        this->checkstatus(::pipe(inpipe));

        std::future<void> future = std::async(
            &This::pipeline, this, invocations, inpipe[INPUT], fdout);

        this->write_from_stream(instream, inpipe[OUTPUT]);
        future.wait();
    }

    ExitStatus PosixProcessProvider::waitpid(PID pid, bool checkstatus) const
    {
        int wstatus = 0;
        int options = 0;
        ::waitpid(pid, &wstatus, options);
        if (checkstatus && (wstatus != 0))
        {
            throw std::system_error(wstatus, std::system_category());
        }
        return wstatus;
    }

    void PosixProcessProvider::write_from_stream(std::istream &stream, FileDescriptor fd) const
    {
        std::vector<char> buffer(CHUNKSIZE);
        while (stream.good())
        {
            stream.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
            ::write(fd, buffer.data(), stream.gcount());
        }
        // logf_debug("Wrote %d bytes from input stream to file descriptor %d",
        //            stream->tellg(),
        //            fd);
    }

    void PosixProcessProvider::trim_pipe(
        FileDescriptor pipefd[2],
        Direction direction,
        FileDescriptor *fd) const
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

    void PosixProcessProvider::close_fds(FileDescriptor pipefd[2]) const
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
        execv(c_argv.front(), c_argv.data());

        // Why are we still here?
        throw std::system_error(errno, std::system_category());
    }

}  // namespace core::platform
