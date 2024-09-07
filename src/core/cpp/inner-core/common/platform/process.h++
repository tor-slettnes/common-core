// -*- c++ -*-
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

namespace core::platform
{
    constexpr uint CHUNKSIZE = 4096;

    using PID = pid_t;
    using ExitStatus = int;
    using FileDescriptor = int;
    using ArgVector = std::vector<std::string>;

    struct Invocation
    {
        ArgVector argv;
        fs::path cwd;
    };

    struct InvocationResult
    {
        PID pid = 0;
        ExitStatus exit_status = 0;
        std::shared_ptr<std::stringstream> stdout;
        std::shared_ptr<std::stringstream> stderr;
    };

    using Invocations = std::vector<Invocation>;
    using InvocationResults = std::vector<InvocationResult>;

    /// @brief Abstract provider for process invocation
    class ProcessProvider : public Provider
    {
        using This = ProcessProvider;
        using Super = Provider;

    protected:
        using Super::Super;

    public:
        /// @fn thread_id
        /// @brief Return an OS-level identifier for the current thread
        virtual PID thread_id() const;

        /// @fn thread_id
        /// @brief Return an OS-level identifier for the current thread
        virtual PID process_id() const;

        /// @fn arg_vector
        /// @brief Convert a variant value of type string or valuelist to argument vector
        virtual ArgVector arg_vector(
            const types::Value &command) const;

        /// @fn shell_command
        /// @brief Return an argument vector to run the specified command line in a shell
        virtual ArgVector shell_command(
            const std::string &command_line) const;

        /// @fn invoke_async
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

        virtual PID invoke_async(
            const ArgVector &argv,
            const fs::path &cwd = {},
            const fs::path &infile = path->devnull(),
            const fs::path &outfile = {},
            const fs::path &errfile = {}) const;

        /// @fn invoke_sync
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
        /// @param[in] errfile
        ///     Redirect standard error (diagnostic output) to the specified file
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual ExitStatus invoke_sync(
            const ArgVector &argv,
            const fs::path &cwd = {},
            const fs::path &infile = path->devnull(),
            const fs::path &outfile = {},
            const fs::path &errfile = {}) const;

        /// @fn invoke_async_pipe
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
            FileDescriptor *fderr = nullptr) const;

        /// @fn capture_pipe
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
        /// @param[out] outstream
        ///     Output stream to which data will be sent from the program's `stdout`.
        ///     Passing in `nullptr` discard the child's standard output.
        /// @param[out] errstream
        ///     Output stream to which data will be sent from the program's `stderr`.
        ///     Passing in `nullptr` discard the child's standard error.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual ExitStatus pipe_capture(
            PID pid,
            FileDescriptor fdin,
            FileDescriptor fdout,
            FileDescriptor fderr,
            std::istream *instream,
            std::ostream *outstream,
            std::ostream *errstream) const;

        /// @fn invoke_capture
        /// @brief Invoke a command with stdin/stdout/stderr capture.
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] instream
        ///     Input stream from which data will be sent to program's `stdin`.
        ///     Passing in `nullptr` closes `fdin`.
        /// @param[out] outstream
        ///     Output stream to which data will be sent from the program's `stdout`.
        ///     Passing in `nullptr` discard the child's standard output.
        /// @param[out] errstream
        ///     Output stream to which data will be sent from the program's `stderr`.
        ///     Passing in `nullptr` discard the child's standard error.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual ExitStatus invoke_capture(
            const ArgVector &argv,
            const fs::path &cwd = {},
            std::istream *instream = nullptr,
            std::ostream *outstream = nullptr,
            std::ostream *errstream = nullptr) const;

        /// @fn invoke_capture
        /// @brief Invoke a command with stdin/stdout/stderr capture.
        /// @param[in] argv
        ///     Argument vector. The first element (index #0) is the full path
        ///     of the program file to invoke.
        /// @param[in] cwd
        ///     Change working directory
        /// @param[in] input
        ///     Text to be sent on `stdin'.
        /// @param[out] output
        ///     Text captured from `stdout`.
        ///     Passing in `nullptr` discards the child's standard output.
        /// @param[out] diag
        ///     Text captured from `stderr`.
        ///     Passing in `nullptr` discards the child's standard error.
        /// @return
        ///     The exit code returned from the process, as returned from `waitpid(2)`.
        /// @exception std::system_error
        ///     An underlying system call failed.

        virtual ExitStatus invoke_capture(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input,
            std::string *output = nullptr,
            std::string *diag = nullptr) const;

        /// @fn invoke_check
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
        /// @param[out] outstream
        ///     Output stream to which data will be sent from the program's `stdout`.
        ///     Passing in `nullptr` discard the child's standard output.
        /// @param[out] errstream
        ///     Output stream to which data will be sent from the program's `stderr`.
        ///     Passing in `nullptr` discard the child's standard error.
        /// @exception std::system_error
        ///     An underlying system call failed, or the process returned a
        ///     non-zero exit status.

        virtual void invoke_check(
            const ArgVector &argv,
            const fs::path &cwd = {},
            std::istream *instream = nullptr,
            std::ostream *outstream = nullptr,
            std::ostream *errstream = nullptr) const;

        /// @fn invoke_check
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
        /// @param[out] output
        ///     Text captured from `stdout`.
        ///     Passing in `nullptr` discards the child's standard output.
        /// @param[out] diag
        ///     Text captured from `stderr`.
        ///     Passing in `nullptr` discards the child's standard error.
        /// @exception std::system_error
        ///     An underlying system call failed, or the process returned a
        ///     non-zero exit status.

        virtual void invoke_check(
            const ArgVector &argv,
            const fs::path &cwd,
            const std::string &input,
            std::string *output = nullptr,
            std::string *diag = nullptr) const;

        /// @fn pipeline
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

        /// @fn pipe_from_stream
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

        /// @fn read
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

        virtual std::size_t read(
            FileDescriptor fd,
            void *buffer,
            std::size_t bufsize) const = 0;

        /// @fn write
        /// @brief
        ///     Write to a file descriptor
        /// @param[in] fd
        ///     Writeable file descriptor
        /// @param[in] buffer
        ///     Buffer containing data to write
        /// @param[in] bufsize
        ///     Bytes to write
        /// @return
        ///     Number of characters written
        /// @exception std::system_error
        ///     Write operation failed

        virtual std::size_t write(
            FileDescriptor fd,
            void *buffer,
            std::size_t length) const = 0;

        /// @fn waitpid
        /// @brief
        ///     Wait for the specified Process ID to exit.
        /// @param[in] checkstatus
        ///     Whether to throw an exception if the command returns a non-zero
        ///     exit status.
        /// @return
        ///     Exit status from the process. Use the `WEXITSTSTUS()` macro
        ///     to convert this to a system error code.
        virtual ExitStatus waitpid(PID pid,
                                   bool checkstatus = false) const = 0;

    protected:
        template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
        inline T checkstatus(T status, const std::string &context = {}) const
        {
            if (status < 0)
            {
                throw std::system_error(errno, std::system_category(), context);
            }
            return status;
        }

        inline FILE *checkstatus(FILE *fp, const std::string &context = {}) const
        {
            if (fp == NULL)
            {
                throw std::system_error(errno, std::system_category(), context);
            }
            return fp;
        }
    };

    /// Global instance, populated with the "best" provider for this system.
    extern ProviderProxy<ProcessProvider> process;

}  // namespace core::platform
