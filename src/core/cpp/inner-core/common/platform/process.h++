// -*- c++ -*-exitsta
//==============================================================================
/// @file process.h++
/// @brief Process invocation - Abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "provider.h++"
#include "path.h++"
#include "types/platform.h++"
#include "types/streamable.h++"

#include <string>
#include <vector>
#include <stdexcept>
#include <array>

namespace core::platform
{
    constexpr uint CHUNKSIZE = 4096;

    using FileDescriptor = int;
    using PID = pid_t;
    using ArgVector = std::vector<std::string>;
    using Pipe = std::array<FileDescriptor, 2>;
    enum PipeDirection
    {
        INPUT = 0,
        OUTPUT = 1
    };

    //==========================================================================
    // ExitStatus

    class ExitStatus : public core::types::Streamable
    {
    public:
        using ptr = std::shared_ptr<ExitStatus>;

    public:
        virtual int combined_code() const;
        virtual int exit_code() const = 0;
        virtual int exit_signal() const = 0;
        virtual bool success() const = 0;
        virtual std::string symbol() const = 0;
        virtual std::string text() const = 0;
        virtual types::TaggedValueList as_tvlist() const;

    protected:
        void to_stream(std::ostream &stream) const override;
    };

    //==========================================================================
    // Invocation

    struct Invocation
    {
        ArgVector argv;
        fs::path cwd;
    };

    using Invocations = std::vector<Invocation>;

    //==========================================================================
    // Invocation

    struct InvocationState
    {
        std::string command;
        PID pid = 0;
        FileDescriptor stdin = -1;
        FileDescriptor stdout = -1;
        FileDescriptor stderr = -1;
    };

    using InvocationStates = std::vector<InvocationState>;

    //==========================================================================
    // InvocationResult

    class InvocationResult : public core::types::Streamable
    {
    public:
        InvocationResult(PID pid = {},
                         ExitStatus::ptr status = {},
                         std::shared_ptr<std::stringstream> stdout = {},
                         std::shared_ptr<std::stringstream> stderr = {});

        int error_code() const;
        std::string error_symbol() const;
        std::string error_text() const;

        std::string stdout_text() const;
        std::string stderr_text() const;
        std::ios::pos_type stdout_size() const;
        std::ios::pos_type stderr_size() const;

        types::TaggedValueList as_tvlist() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        PID pid = 0;
        ExitStatus::ptr status;
        std::shared_ptr<std::stringstream> stdout;
        std::shared_ptr<std::stringstream> stderr;
    };

    using InvocationResults = std::vector<InvocationResult>;

    //==========================================================================
    // ProcessProvider

    class ProcessProvider : public Provider
    {
        using This = ProcessProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        /// @brief Return an OS-level identifier for the current thread
        virtual PID thread_id() const;

        /// @brief Return an OS-level identifier for the current thread
        virtual PID process_id() const;

        /// @brief Convert a variant value of type string or valuelist to argument vector
        virtual ArgVector arg_vector(
            const types::Value &command) const;

        /// @brief Return an argument vector to run the specified command line in a shell
        virtual ArgVector shell_command(
            const std::string &command_line) const;

        /// @brief Clone this process
        /// @return The process ID of the child if we are the parent, otherwise 0.
        virtual PID fork_process() const = 0;

        /// @brief Invoke a command, and immediately return its PID
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] infile
        ///     Redirect standard input from the specified file
        /// @param[in] outfile
        ///     Redirect standard output to the specified file
        /// @param[in] errfile
        ///     Redirect standard error (diagnostic output) to the specified file
        /// @param[in] detach
        ///     Start a new detached process group, unlinking the process from its parent.
        /// @return
        ///     The process ID of the child. (Note that any subprocesses spawned
        ///     from the child itself are not captured).
        /// @exception std::system_error
        ///     An underlying system call failed.
        ///
        /// The `cwd`, `stdin`, `stdout` and `stderr` arguments are evaluated in the
        /// child process, before launching the target executable. Each may be
        /// empty, in which case the corresponding value is inherited from this
        /// (parent) process.

        virtual PID invoke_async_fileio(
            const ArgVector &argv,
            const fs::path &cwd = {},
            const fs::path &infile = path->devnull(),
            const fs::path &outfile = {},
            const fs::path &errfile = {},
            bool detach = false) const;

        /// @brief Invoke a command, and wait (block this thread) until it exits.
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] infile
        ///     Redirect standard input from the specified file
        /// @param[in] outfile
        ///     Redirect standard output to the specified file
        /// @param[in] detach
        ///     Start a new detached process group, unlinking the process from its parent.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual ExitStatus::ptr invoke_sync_fileio(
            const ArgVector &argv,
            const fs::path &cwd = {},
            const fs::path &infile = path->devnull(),
            const fs::path &outfile = {},
            const fs::path &errfile = {}) const;

        /// @brief Invoke a command with UNIX pipes
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[out] fdin
        ///     Writable UNIX file descriptor of a pipe to the program's `stdin`.
        ///     Passing in `nullptr` closes the child's standard input.
        /// @param[out] fdout
        ///     Readable UNIX file descriptor of a pipe from the program's `stdout`.
        ///     Passing in `nullptr` closes the child's standard output.
        /// @param[out] fderr
        ///     Readable UNIX file descriptor of a pipe from the program's `stderr`.
        ///     Passing in `nullptr` closes the child's standard error.
        /// @param[in] detach
        ///     Start a new detached process group, unlinking the process from its parent.
        /// @return
        ///     The process ID of the child.
        /// @exception std::system_error
        ///     An underlying system call failed.
        /// @note
        ///     It is the caller's responsibility to close the file descriptors
        ///     `fdin`, `fdout`, and `fderr`, if provided.

        virtual PID invoke_async_pipe(
            const ArgVector &argv,
            const fs::path &cwd = {},
            FileDescriptor *fdin = nullptr,
            FileDescriptor *fdout = nullptr,
            FileDescriptor *fderr = nullptr,
            bool detach = false) const;

        /// @brief Communicate with a child process through an established pipe
        /// @param[in] pid
        ///     Process ID, as returned from invoke_async_pipe
        /// @param[out] fdin
        ///     Writable UNIX file descriptor of a pipe to the program's `stdin`.
        /// @param[out] fdout
        ///     Readable UNIX file descriptor of a pipe from the program's `stdout`.
        /// @param[out] fderr
        ///     Readable UNIX file descriptor of a pipe from the program's `stderr`.
        /// @param[in] instream
        ///     Input stream from which data will be sent to program's `stdin`.
        ///     Passing in `nullptr` closes `fdin`.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual InvocationResult pipe_capture(
            PID pid,
            FileDescriptor fdin,
            FileDescriptor fdout,
            FileDescriptor fderr,
            std::istream *instream) const;

        /// @brief
        ///     Invoke a command with stdin/stdout/stderr capture.
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] instream
        ///     Input stream from which data will be sent to program's `stdin`.
        ///     Passing in `nullptr` closes `fdin`.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual InvocationResult invoke_capture(
            const ArgVector &argv,
            const fs::path &cwd = {},
            std::istream *instream = nullptr) const;

        /// @brief
        ///     Invoke a command with stdin/stdout/stderr capture.
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] input
        ///     Text to be sent on `stdin'.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual InvocationResult invoke_capture(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input) const;

        /// @brief
        ///     Invoke a command with stdio capture,
        ///     throw SystemError on non-zero exit status
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] instream
        ///     Input stream from which data will be sent to program's `stdin`.
        ///     Passing in `nullptr` closes `fdin`.
        /// @exception std::system_error
        ///     An underlying system call failed, or the process returned a
        ///     non-zero exit status.

        virtual void invoke_check(
            const ArgVector &argv,
            const fs::path &cwd = {},
            std::istream *instream = nullptr) const;

        /// @brief
        ///     Invoke a command with stdio capture,
        ///     throw SystemError on non-zero exit status
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] input
        ///     Text to be sent on `stdin'.
        /// @exception std::system_error
        ///     An underlying system call failed, or the process returned a
        ///     non-zero exit status.

        virtual void invoke_check(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input) const;

        /// @brief
        ///     Invoke multiple commands in parallel, with standard input to the
        ///     first command from the provided input stream.  Standard output
        ///     from each command is then piped to standard input of the next.
        /// @param[in] invocations
        ///     A vector of argument vectors and associated current working
        ///     directories.
        /// @param[in] instream
        ///     Input stream from which data is piped to standard input for the
        ///     first command.
        /// @param[in] checkstatus
        ///     Whether to throw an exception if any command returns a non-zero
        ///     exit status.  Note that this means detailed results from each
        ///     command will not be returned.
        /// @return
        ///     Process ID, exit status, and diagnostic output (stderr) from
        ///     each pcocess in the pipeline.  Standard output (stdout) is also
        ///     captured from the last process.

        virtual InvocationResults pipe_from_stream(
            const Invocations &invocations,
            std::istream &instream,
            bool checkstatus = false) const;

        /// @brief
        ///     Invoke multiple commands in parallel, with standard output from
        ///     each piped to standard input of the next.
        /// @param[in] invocations
        ///     A vector of argument vectors and associated current working
        ///     directories.
        /// @param[in] fdin
        ///     Readable file descriptor redirected to standard input for the
        ///     first command.
        /// @param[in] checkstatus
        ///     Whether to throw an exception if any command returns a non-zero
        ///     exit status.  Note that this means detailed results from each
        ///     command will not be returned.
        /// @return
        ///     Process ID, exit status, and diagnostic output (stderr) from
        ///     each pcocess in the pipeline.  Standard output (stdout) is also
        ///     captured from the last process.

        virtual InvocationResults pipeline(
            const Invocations &invocations,
            FileDescriptor fdin,
            bool checkstatus = false) const;

        virtual InvocationStates create_pipeline(
            const Invocations &invocations,
            FileDescriptor fdin) const = 0;

        virtual InvocationResults capture_pipeline(
            const InvocationStates &states,
            bool checkstatus) const = 0;

        /// @brief
        ///     Create a pipe, comprising a pair of connected file descriptors
        ///     for reading and writing, respectively.
        /// @return
        ///     A pair of file descriptors for reading and writing, respectively.
        ///     Data written to Pipe[OUTPUT] can subsequently be read from
        ///     Pipe[INPUT].
        virtual Pipe create_pipe() const = 0;

        /// @brief
        ///     Open a file for reading
        /// @param[in] filename
        ///     Path to file which will be opeend
        /// @return
        ///     Readable file descriptor to the open file.

        virtual FileDescriptor open_read(
            const fs::path &filename) const = 0;

        /// @brief
        ///     Open a file for writing
        /// @param[in] filename
        ///     Path to file which will be opeend
        /// @param[in] create_mode
        ///     UNIX file mode bits for the created file
        /// @return
        ///     Writable file descriptor to the open file.

        virtual FileDescriptor open_write(
            const fs::path &filename,
            int create_mode = 0644) const = 0;

        /// @brief
        ///     Close an open file descriptor
        /// @param[in] fd
        ///     Readable file descriptor

        virtual void close_fd(
            FileDescriptor fd) const = 0;

        /// @brief
        ///     Read from a file descriptor
        /// @param[in] fd
        ///     Readable file descriptor
        /// @param[in] buffer
        ///     Buffer to hold received data
        /// @param[in] bufsize
        ///     Maximum number of bytes to read
        /// @return
        ///     Number of characters read
        /// @exception std::system_error
        ///     Read operation failed

        virtual std::size_t read_fd(
            FileDescriptor fd,
            void *buffer,
            std::size_t bufsize) const = 0;

        /// @brief
        ///     Write to a file descriptor
        /// @param[in] fd
        ///     Writeable file descriptor
        /// @param[in] buffer
        ///     Buffer containing data to write
        /// @param[in] size
        ///     Number of bytes to write
        /// @return
        ///     Number of characters written
        /// @exception std::system_error
        ///     Write operation failed

        virtual std::size_t write_fd(
            FileDescriptor fd,
            const void *buffer,
            std::size_t size) const = 0;

        /// @brief
        ///     Write to a file descriptor from a C++ input stream
        /// @param[in] stream
        ///     Input stream from which data is read
        /// @param[in] fd
        ///     Writeable file descriptor to which data is written

        virtual void write_from_stream(
            std::istream *stream,
            FileDescriptor fd) const;

        /// @brief
        ///    Close both input/output file descriptors of a pipe

        virtual void close_pipe(
            const Pipe &pipe) const = 0;

        /// @brief
        ///     Wait for the specified Process ID to exit.
        /// @param[in] pid
        ///     Process ID
        /// @param[in] checkstatus
        ///     Whether to throw an exception if the command returns a non-zero
        ///     exit status.
        /// @return
        ///     Exit status from the process. Use the `WEXITSTSTUS()` macro
        ///     to convert this to a system error code.
        virtual ExitStatus::ptr waitpid(
            PID pid,
            bool checkstatus = false) const = 0;
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<ProcessProvider> process;

}  // namespace core::platform
