// -*- c++ -*-
//==============================================================================
/// @file posix-process.c++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-process.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

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
        int wstatus = 0;
        ::waitpid(pid, &wstatus, 0);
        return wstatus;
    }

    // PID PosixProcessProvider::invoke_async_pipe(
    //     const ArgVector &argv,
    //     const fs::path &cwd,
    //     FileDescriptor *fdin,
    //     FileDescriptor *fdout,
    //     FileDescriptor *fderr) const
    // {
    //     if (argv.empty())
    //     {
    //         throw std::invalid_argument("Missing command");
    //     }

    //     FileDescriptor inpipe[2], outpipe[2], errpipe[2];

    //     this->checkstatus(::pipe(inpipe));
    //     this->checkstatus(::pipe(outpipe));
    //     this->checkstatus(::pipe(errpipe));

    //     PID pid = this->checkstatus(fork());

    //     if (pid == 0)
    //     {
    //         // Child uses inpipe/outpipe/errpipe as stdin/stdout/sderr.
    //         this->checkstatus(dup2(inpipe[INPUT], STDIN_FILENO));
    //         this->checkstatus(dup2(outpipe[OUTPUT], STDOUT_FILENO));
    //         this->checkstatus(dup2(errpipe[OUTPUT], STDERR_FILENO));

    //         this->close_fds(inpipe);
    //         this->close_fds(outpipe);
    //         this->close_fds(errpipe);

    //         this->execute(argv, cwd);
    //     }
    //     else
    //     {
    //         // Parent writes to inpipe and reads from outpipe and errpipe.
    //         this->trim_pipe(inpipe, OUTPUT, fdin);
    //         this->trim_pipe(outpipe, INPUT, fdout);
    //         this->trim_pipe(errpipe, INPUT, fderr);
    //     }

    //     return pid;
    // }

    PID PosixProcessProvider::invoke_async_pipe(
        const ArgVector &argv,
        const fs::path &cwd,
        FileDescriptor *fdin,
        FileDescriptor *fdout,
        FileDescriptor *fderr) const
    {
        Pipe inpipe = {-1, -1};

        if (fdin)
        {
            inpipe = this->create_pipe();
            *fdin = inpipe[OUTPUT];
        }

        return this->invoke_async_from_fd(argv, cwd, inpipe[INPUT], fdout, fderr);
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
        std::vector<struct pollfd> pfds = {
            {fdin, POLLOUT, 0},
            {fdout, POLLIN, 0},
            {fderr, POLLIN, 0},
        };

        for (uint i = 0; i < pfds.size(); i++)
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
                ::close(fdin);
                pfds[STDIN_FILENO].fd = -1;
                pfds[STDIN_FILENO].events = 0;
            }

            this->checkstatus(::poll(pfds.data(), pfds.size(), -1));

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

        for (uint i = 0; i < pfds.size(); i++)
        {
            this->close_fd(pfds[i].fd);
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

    InvocationResults PosixProcessProvider::pipeline(
        const Invocations &invocations,
        FileDescriptor fdin,
        bool checkstatus) const
    {
        FileDescriptor pipe_fd = fdin;
        std::vector<struct pollfd> pfds;
        InvocationResults results;
        results.reserve(invocations.size());

        // Invoke commands
        for (const Invocation &invocation : invocations)
        {
            FileDescriptor stderr_fd;
            PID pid = this->invoke_async_from_fd(
                invocation.argv,
                invocation.cwd,
                pipe_fd,
                &pipe_fd,
                &stderr_fd);

            results.push_back({.pid = pid});

            pfds.push_back({
                .fd = stderr_fd,
                .events = POLLIN,
                .revents = 0,
            });
        }

        // Append stdout from the last command in the pipeline to
        // the file descriptors that we monitor.

        pfds.push_back({
            .fd = pipe_fd,
            .events = POLLIN,
            .revents = 0,
        });

        this->poll_outputs(pfds, invocations, &results);
        this->wait_results(invocations, checkstatus, &results);
        return results;
    }

    InvocationResults PosixProcessProvider::pipe_from_stream(
        const Invocations &invocations,
        std::istream &instream,
        bool checkstatus) const
    {
        Pipe inpipe = this->create_pipe();
        logf_debug("Created pipe from stream, %d -> %d", inpipe[OUTPUT], inpipe[INPUT]);

        std::future<InvocationResults> future = std::async(
            &This::pipeline, this, invocations, inpipe[INPUT], checkstatus);

        this->write_from_stream(instream, inpipe[OUTPUT]);
        this->close_fd(inpipe[OUTPUT]);
        logf_debug("Closed pipe output %d; waiting for pipeline", inpipe[OUTPUT]);
        InvocationResults results = future.get();
        logf_debug("Pipeline finished");
        return results;
    }

    ExitStatus PosixProcessProvider::waitpid(PID pid, bool checkstatus) const
    {
        int wstatus = 0;
        int options = 0;
        ::waitpid(pid, &wstatus, options);
        if (checkstatus && (WEXITSTATUS(wstatus) != EXIT_SUCCESS))
        {
            throw std::system_error(wstatus, std::system_category());
        }
        return wstatus;
    }

    PID PosixProcessProvider::invoke_async_from_fd(
        const ArgVector &argv,
        const fs::path &cwd,
        FileDescriptor fdin,
        FileDescriptor *fdout,
        FileDescriptor *fderr) const
    {
        if (argv.empty())
        {
            throw std::invalid_argument("Missing command");
        }

        Pipe outpipe = this->create_pipe();
        Pipe errpipe = this->create_pipe();
        PID pid = this->checkstatus(fork());

        if (pid == 0)
        {
            // Child uses inpipe as input and outpipe/errpipe as stdout/sderr.
            if (fdin > 0)
            {
                this->checkstatus(dup2(fdin, STDIN_FILENO));
            }
            // this->close_pipe(inpipe);

            this->checkstatus(dup2(outpipe[OUTPUT], STDOUT_FILENO));
            // this->close_pipe(outpipe);

            this->checkstatus(dup2(errpipe[OUTPUT], STDERR_FILENO));
            // this->close_pipe(errpipe);

            this->execute(argv, cwd);
        }
        else
        {
            // Parent writes to inpipe and reads from outpipe and errpipe.
            this->close_fd(fdin);
            this->trim_pipe(outpipe, INPUT, fdout);
            this->trim_pipe(errpipe, INPUT, fderr);

            logf_debug("Forked pid=%d; cwd=%s, argv=%s", pid, cwd, argv);
        }

        return pid;
    }

    void PosixProcessProvider::write_from_stream(std::istream &stream, FileDescriptor fd) const
    {
        std::vector<char> buffer(CHUNKSIZE);
        std::size_t count = 0;
        while (stream.good())
        {
            stream.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
            count += stream.gcount();
            ::write(fd, buffer.data(), stream.gcount());
        }
        logf_debug("Wrote %d bytes from input stream to file descriptor %d", count, fd);
    }

    void PosixProcessProvider::trim_pipe(
        const Pipe &pipe,
        Direction direction,
        FileDescriptor *fd) const
    {
        this->close_fd(pipe[!direction]);

        if (fd)
        {
            *fd = pipe[direction];
        }
        else
        {
            this->close_fd(pipe[direction]);
        }
    }

    void PosixProcessProvider::close_pipe(const Pipe &pipe) const
    {
        this->close_fd(pipe[INPUT]);
        this->close_fd(pipe[OUTPUT]);
    }

    void PosixProcessProvider::close_fd(FileDescriptor fd) const
    {
        if (fd >= 0)
        {
            this->checkstatus(::close(fd));
            This::open_fds.erase(fd);
        }
    }

    Pipe PosixProcessProvider::create_pipe() const
    {
        Pipe pipe;
        this->checkstatus(::pipe(pipe.data()));
        This::open_fds.insert(pipe[INPUT]);
        This::open_fds.insert(pipe[OUTPUT]);
        return pipe;
    }

    void PosixProcessProvider::execute(ArgVector argv, const fs::path &cwd) const
    {
        if (!cwd.empty())
        {
            this->checkstatus(::chdir(cwd.string().data()));
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

        // Close any open pipes (these should already have been reassigned to
        // stdin/stdout/stderr).
        for (FileDescriptor fd : This::open_fds)
        {
            ::close(fd);
        }

        // Here we go. This should be the last thing we do. Goodbye!
        execv(c_argv.front(), c_argv.data());

        // Why are we still here?
        throw std::system_error(errno, std::system_category(), argv.front());
    }

    void PosixProcessProvider::poll_outputs(
        const std::vector<struct pollfd> &pfds,
        const Invocations &invocations,
        InvocationResults *results) const
    {
        std::unordered_set<FileDescriptor> open_fds;
        for (const struct pollfd &pfd : pfds)
        {
            open_fds.insert(pfd.fd);
            fcntl(pfd.fd, F_SETFL, O_NONBLOCK);
        }

        // Collect anything printed on `stderr` from each subprocess,
        // as well as on `stdout` from the last process in the pipeline.
        // We do this with `poll()` rather than sequentially in order to
        // avoid deadlocks in case the text from a given process fills
        // up the corresponding pipe's temporary buffer.

        for (bool received = true; received || !open_fds.empty(); received = false)
        {
            logf_trace("Polling %d FDs", pfds.size());
            this->checkstatus(
                ::poll(const_cast<struct pollfd *>(pfds.data()), pfds.size(), -1));

            for (uint c = 0; c < invocations.size(); c++)
            {
                received |= this->check_poll(pfds.at(c),
                                             invocations.at(c),
                                             results->at(c),
                                             "stderr",
                                             &results->at(c).stderr,
                                             &open_fds);
            }

            received |= this->check_poll(pfds.back(),
                                         invocations.back(),
                                         results->back(),
                                         "stdout",
                                         &results->back().stdout,
                                         &open_fds);
        }
    }

    bool PosixProcessProvider::check_poll(
        const struct pollfd &pfd,
        const Invocation &invocation,
        const InvocationResult &result,
        const std::string &stream_name,
        std::shared_ptr<std::stringstream> *outstream,
        std::unordered_set<FileDescriptor> *open_fds) const
    {
        static std::vector<char> outbuf(CHUNKSIZE);
        bool received = false;

        if (pfd.revents & POLLIN)
        {
            int nchars = this->checkstatus(
                ::read(pfd.fd,
                       outbuf.data(),
                       outbuf.size()));

            logf_trace("Captured %d bytes from PID %d stderr",
                       nchars,
                       result.pid);

            if (!*outstream)
            {
                *outstream = std::make_shared<std::stringstream>();
            }

            (*outstream)->write(outbuf.data(), nchars);
            received = true;
        }
        else if (pfd.revents)
        {
            logf_trace("No longer monitoring %s from PID %d [%s]",
                       stream_name,
                       result.pid,
                       invocation.argv.front());
            open_fds->erase(pfd.fd);
        }
        return received;
    }

    void PosixProcessProvider::wait_results(
        const Invocations &invocations,
        bool checkstatus,
        InvocationResults *results) const
    {
        // Now wait for all subprocesses to complete.  We capture the error from
        // the first process to exit with a non-zero code, if any.
        std::exception_ptr eptr;
        for (uint c = 0; c < invocations.size(); c++)
        {
            const Invocation &invocation = invocations.at(c);
            InvocationResult &result = results->at(c);

            logf_trace("Waiting for PID %d: %s", result.pid, invocation.argv);
            ::waitpid(result.pid, &result.exit_status, 0);
            if (ExitStatus err = WEXITSTATUS(result.exit_status))
            {
                if (checkstatus && !eptr)
                {
                    eptr = std::make_exception_ptr(
                        std::system_error(err,
                                          std::system_category(),
                                          invocation.argv.front()));
                }
                logf_info("PID %d [%s] returned non-zero exit status %d (%s)",
                          result.pid,
                          invocation.argv.front(),
                          err,
                          core::platform::symbols->errno_name(err));
            }
            else
            {
                logf_debug("PID %d [%s] returned successfully",
                           result.pid,
                           invocation.argv.front());
            }
        }

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

    std::unordered_set<FileDescriptor> PosixProcessProvider::open_fds;

}  // namespace core::platform
