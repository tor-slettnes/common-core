// -*- c++ -*-
//==============================================================================
/// @file posix-process.c++
/// @brief Process invocation - POSIX implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-process.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <utility>

#define ELEMENTS(array) (sizeof(array) / sizeof(array[0]))

namespace core::platform
{

    //==========================================================================
    // PosixExitStatus

    PosixExitStatus::PosixExitStatus(int raw_code)
        : ExitStatus(),
          raw_code(raw_code)
    {
    }

    int PosixExitStatus::exit_code() const
    {
        return WIFEXITED(this->raw_code) ? WEXITSTATUS(this->raw_code) : EXIT_SUCCESS;
    }

    int PosixExitStatus::exit_signal() const
    {
        return WIFSIGNALED(this->raw_code) ? WTERMSIG(this->raw_code) : EXIT_SUCCESS;
    }

    bool PosixExitStatus::success() const
    {
        return this->raw_code == EXIT_SUCCESS;
    }

    std::string PosixExitStatus::symbol() const
    {
        if (uint code = this->exit_code())
        {
            return symbols->errno_name(code);
        }
        else if (uint signal = this->exit_signal())
        {
            const char *abbrev = ::sigabbrev_np(signal);
            return str::format("SIG%s", abbrev ? abbrev : "_UNKNOWN");
        }
        else
        {
            return {};
        }
    }

    std::string PosixExitStatus::text() const
    {
        if (int code = this->exit_code())
        {
            return symbols->errno_string(code);
        }
        else if (int signal = this->exit_signal())
        {
            const char *descr = ::sigdescr_np(this->exit_signal());
            return descr ? descr : "";
        }
        else
        {
            return {};
        }
    }

    //==========================================================================
    // PosixProcessProvider

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

    PID PosixProcessProvider::invoke_async_fileio(
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

    ExitStatus::ptr PosixProcessProvider::invoke_sync_fileio(
        const ArgVector &argv,
        const fs::path &cwd,
        const fs::path &infile,
        const fs::path &outfile,
        const fs::path &errfile) const
    {
        PID pid = this->invoke_async_fileio(argv, cwd, infile, outfile, errfile);
        return this->waitpid(pid);
    }

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

    InvocationResult PosixProcessProvider::pipe_capture(
        PID pid,
        FileDescriptor fdin,
        FileDescriptor fdout,
        FileDescriptor fderr,
        std::istream *instream) const
    {
        std::vector<struct pollfd> pfds = {
            {fdin, POLLOUT, 0},
            {fdout, POLLIN, 0},
            {fderr, POLLIN, 0},
        };

        std::unordered_set<FileDescriptor> open_fds;
        for (uint i = 0; i < pfds.size(); i++)
        {
            if (FileDescriptor fd = pfds[i].fd; fd >= 0)
            {
                open_fds.insert(fd);
                fcntl(fd, F_SETFL, O_NONBLOCK);
            }
        }

        // if (!instream)
        // {
        //     open_fds.erase(pfds[STDIN_FILENO].fd);
        //     this->close_poll(&pfds[STDIN_FILENO]);
        // }

        InvocationResult result(pid);
        std::vector<char> buf(CHUNKSIZE);
        int wstatus = 0;

        while ((::waitpid(pid, &wstatus, WNOHANG) == 0) && !open_fds.empty())
        {
            this->checkstatus(::poll(pfds.data(), pfds.size(), -1));

            if (int stdin_event = pfds[STDIN_FILENO].revents)
            {
                bool sent = false;
                if (((stdin_event & POLLOUT) != 0) && instream)
                {
                    instream->read(buf.data(), buf.size());
                    if (std::streamsize nbytes = instream->gcount())
                    {
                        this->write_fd(fdin, buf.data(), nbytes);
                        sent = true;
                    }
                }

                if (!sent)
                {
                    open_fds.erase(pfds[STDIN_FILENO].fd);
                    this->close_poll(&pfds[STDIN_FILENO]);
                }
            }

            this->check_poll(
                str::format("PID %d stdout", pid),
                &pfds[STDOUT_FILENO],
                &result.stdout,
                &open_fds);

            this->check_poll(
                str::format("PID %d stderr", pid),
                &pfds[STDERR_FILENO],
                &result.stderr,
                &open_fds);
        }

        for (uint i = 0; i < pfds.size(); i++)
        {
            this->close_fd(pfds[i].fd);
        }

        result.status = std::make_shared<PosixExitStatus>(wstatus);
        logf_debug("Captured from PID %d: %s", pid, result);
        return result;
    }

    InvocationStates PosixProcessProvider::create_pipeline(
        const Invocations &invocations,
        FileDescriptor fdin) const
    {
        FileDescriptor pipe_fd = fdin;
        InvocationStates states;
        states.reserve(invocations.size());

        // Invoke commands
        for (const Invocation &invocation : invocations)
        {
            InvocationState &state = states.emplace_back();
            state.command = invocation.argv.front();
            state.stdin = pipe_fd;
            state.pid = this->invoke_async_from_fd(
                invocation.argv,
                invocation.cwd,
                state.stdin,
                &state.stdout,
                &state.stderr);

            pipe_fd = state.stdout;
        }
        return states;
    }

    InvocationResults PosixProcessProvider::capture_pipeline(
        const InvocationStates &states,
        bool checkstatus) const
    {
        InvocationResults results;
        this->poll_outputs(states, &results);
        this->wait_results(states, &results, checkstatus);
        return results;
    }

    Pipe PosixProcessProvider::create_pipe() const
    {
        Pipe pipe;
        this->checkstatus(::pipe(pipe.data()));
        This::open_fds.insert(pipe[INPUT]);
        This::open_fds.insert(pipe[OUTPUT]);
        return pipe;
    }

    ExitStatus::ptr PosixProcessProvider::waitpid(PID pid, bool checkstatus) const
    {
        int wstatus = 0;
        int options = 0;
        ::waitpid(pid, &wstatus, options);
        ExitStatus::ptr status = std::make_shared<PosixExitStatus>(wstatus);

        if (checkstatus && status->combined_code())
        {
            throw core::exception::InvocationError(
                core::str::format("PID %d", pid),
                status);
        }

        return status;
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

        logf_trace("Invoking command, cwd=%s: %s", cwd, argv);

        PID pid = ::fork();

        if (pid == 0)
        {
            // Child uses inpipe as input and outpipe/errpipe as stdout/sderr.
            this->reassign_fd(fdin, STDIN_FILENO);
            this->reassign_fd(outpipe[OUTPUT], STDOUT_FILENO);
            this->reassign_fd(errpipe[OUTPUT], STDERR_FILENO);
            this->execute(argv, cwd);
        }
        else
        {
            // Parent writes to inpipe and reads from outpipe and errpipe.
            this->close_fd(fdin);

            if (pid > 0)
            {
                this->trim_pipe(outpipe, INPUT, fdout);
                this->trim_pipe(errpipe, INPUT, fderr);
            }
            else
            {
                this->close_pipe(outpipe);
                this->close_pipe(errpipe);
            }
        }

        return this->checkstatus(pid);
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
        // throw std::system_error(errno, std::system_category(), argv.front());

        // Why are we still here?
        perror(c_argv.front());
        std::quick_exit(errno);
    }

    void PosixProcessProvider::trim_pipe(
        const Pipe &pipe,
        PipeDirection direction,
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

    FileDescriptor PosixProcessProvider::open_read(
        const fs::path &filename) const
    {
        FileDescriptor fd = this->checkstatus(
            ::open(filename.c_str(), O_RDONLY));

        This::open_fds.insert(fd);
        return fd;
    }

    FileDescriptor PosixProcessProvider::open_write(
        const fs::path &filename,
        int create_mode) const
    {
        FileDescriptor fd = this->checkstatus(
            ::open(filename.c_str(), O_WRONLY | O_CREAT, create_mode));

        This::open_fds.insert(fd);
        return fd;
    }

    void PosixProcessProvider::close_fd(
        FileDescriptor fd) const
    {
        if (fd >= 0)
        {
            ::close(fd);
            This::open_fds.erase(fd);
        }
    }

    std::size_t PosixProcessProvider::read_fd(
        FileDescriptor fd,
        void *buffer,
        std::size_t bufsize) const
    {
        return this->checkstatus(::read(fd, buffer, bufsize));
    }

    std::size_t PosixProcessProvider::write_fd(
        FileDescriptor fd,
        const void *buffer,
        std::size_t size) const
    {
        return this->checkstatus(::write(fd, buffer, size));
    }

    void PosixProcessProvider::reassign_fd(FileDescriptor from,
                                           FileDescriptor to) const
    {
        if (from < 0)
        {
            this->close_fd(to);
        }
        else if (from != to)
        {
            this->checkstatus(::dup2(from, to));
            this->close_fd(from);
        }
    }

    void PosixProcessProvider::close_pipe(const Pipe &pipe) const
    {
        this->close_fd(pipe[INPUT]);
        this->close_fd(pipe[OUTPUT]);
    }

    void PosixProcessProvider::close_poll(
        struct pollfd *pfd) const
    {
        this->close_fd(pfd->fd);
        pfd->fd = -1;
        pfd->events = 0;
    }

    void PosixProcessProvider::poll_outputs(
        const InvocationStates &states,
        InvocationResults *results) const
    {
        results->clear();
        results->reserve(states.size());

        // Monitor anything printed on `stderr` from each subprocess,
        // as well as on `stdout` from the last process in the pipeline.
        // We do this with `poll()` rather than sequentially in order to
        // avoid deadlocks in case the text from a given process fills
        // up the corresponding pipe's temporary buffer.

        std::vector<struct pollfd> pfds;
        pfds.reserve(states.size());
        for (const InvocationState &state : states)
        {
            results->push_back(state.pid);
            pfds.push_back({
                .fd = state.stderr,
                .events = POLLIN,
                .revents = 0,
            });
        }

        pfds.push_back({
            .fd = states.back().stdout,
            .events = POLLIN,
            .revents = 0,
        });

        std::unordered_set<FileDescriptor> open_fds;
        for (const struct pollfd &pfd : pfds)
        {
            open_fds.insert(pfd.fd);
            fcntl(pfd.fd, F_SETFL, O_NONBLOCK);
        }

        while (!open_fds.empty())
        {
            logf_trace("Polling %d FDs", pfds.size());
            this->checkstatus(
                ::poll(const_cast<struct pollfd *>(pfds.data()), pfds.size(), -1));

            for (uint c = 0; c < states.size(); c++)
            {
                this->check_poll(
                    str::format("PID %d [%s] stderr", states.at(c).pid, states.at(c).command),
                    &pfds.at(c),
                    &results->at(c).stderr,
                    &open_fds);
            }

            this->check_poll(
                str::format("PID %d [%s] stdout", states.back().pid, states.back().command),
                &pfds.back(),
                &results->back().stdout,
                &open_fds);
        }
    }

    bool PosixProcessProvider::check_poll(
        const std::string &stream_name,
        struct pollfd *pfd,
        std::shared_ptr<std::stringstream> *outstream,
        std::unordered_set<FileDescriptor> *open_fds) const
    {
        static std::vector<char> outbuf(CHUNKSIZE);
        bool received = false;

        if (pfd->revents & POLLIN)
        {
            int nchars = this->read_fd(pfd->fd, outbuf.data(), outbuf.size());
            std::string_view text(outbuf.data(), nchars);

            logf_trace("Captured %d bytes from %s: %s",
                       nchars,
                       stream_name,
                       text);

            (*outstream)->write(outbuf.data(), nchars);
            received = true;
        }
        else if (pfd->revents)
        {
            logf_trace("No longer monitoring %s",
                       stream_name);

            if (open_fds)
            {
                open_fds->erase(pfd->fd);
            }
            this->close_poll(pfd);
        }
        return received;
    }

    void PosixProcessProvider::wait_results(
        const InvocationStates &states,
        InvocationResults *results,
        bool checkstatus) const
    {
        // Now wait for all subprocesses to complete.  We capture the error from
        // the first process to exit with a non-zero code, if any.
        std::exception_ptr eptr;
        for (uint c = 0; c < states.size(); c++)
        {
            const InvocationState &state = states.at(c);
            InvocationResult &result = results->at(c);
            logf_trace("Waiting for PID %d: %s", state.pid, state.command);

            result.status = this->waitpid(state.pid);

            if (!result.status->success())
            {
                if (checkstatus && !eptr)
                {
                    eptr = std::make_exception_ptr<core::exception::InvocationError>({
                        state.command,
                        result.error_code(),
                        result.error_symbol(),
                        result.error_text(),
                    });
                }
            }
        }

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

    std::set<FileDescriptor> PosixProcessProvider::open_fds;

}  // namespace core::platform
