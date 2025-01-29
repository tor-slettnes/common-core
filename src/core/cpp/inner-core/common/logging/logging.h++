/// -*- c++ -*-
//==============================================================================
/// @file logging.h++
/// @brief Logging interface
/// @author Tor Slettnes <tor@slett.net>
///
/// Logging framework, mainly comprising these classes
///  * `Message()`, representing a log entry
///  * `MessageBuilder()`, derived from `Message()` to construct a log entry
///     from `std::ostream` compatible elements (i.e. objects that support the
///     `<<` operator), optionally with an initial format string.
///  * `LogSink()`, an abstract backend for backends that send the message
///    to specific destination (log file, json file, syslog, etc).
///  * `LogDispatcher()` to distribute a message to applicable sinks.
///  * Wrapper macros @b log_message(), @b log_trace(), @b log_debug(), ...,
/// @b log_error(), to create, build, and dispatch the message in one call.
///  * Wrapper macros @b logf_message(), @b logf_trace(), @b logf_debug(), ...,
/// @b logf_error(), to construct and log in place a message from a format
///    template followed by corresponding arguments.
///
/// @b Usage
///  * Instantiate one or more backends ("sinks").  This is normally done by
///    indirectly by instantiating
///    [argparse::ClientOptions](../argparse/client.h++) or
///    [argparse::ServerOptions](../argparse/server.h++), which in turn creates
///    desired sinks based on selected command line options.
///  * To log a message in a single call, invoke one of
///    @code
///      log_message(status::Level::LEVELNAME, arg, ...);
///      log_trace(arg, ...);
///      log_debug(arg, ...);
///      log_info(arg, ...);
///      log_notice(arg, ...);
///      log_warning(arg, ...);
///      log_error(arg, ...);
///      log_critical(arg, ...);
///      log_fatal(arg, ...);
///    @endcode
///    Or, using a format template:
///    @code
///      logf_message(status::Level::LEVELNAME, format, ...);
///      logf_trace(format, ...);
///      logf_debug(format, ...);
///      logf_info(format, ...);
///      logf_notice(format, ...);
///      logf_warning(format, ...);
///      logf_error(format, ...);
///      logf_critical(format, ...);
///      logf_fatal(format, ...);
///    @endcode
///  * Alternatively, to build a message in steps:
///    @code
///      core::Logging::MessageBuilder::ptr msg = create_log_message(status::Level::LEVELNAME);
///      *msg << arg << ... ;
///      ...
///      msg->submit();
///    @endcode
/// @note
///     For performance reasons, the output string is not actually generated
///     unless there exists at least one sink with @a threshold >= @a LEVELNAME.
///     For this reason, it is preferable to pass raw components on the message
///     as they are, so long as they do or can be made to support C++ output
///     streams. For an example, see the mix-in class
///     [types::Streamable](../types/streamable.h++).
//==============================================================================

#pragma once
#include "message/builder.h++"
#include "dispatchers/dispatcher.h++"

#include <cassert>

//==============================================================================
// Wrapper macros for generating & logging a message in one step.  We use macros
// rather than function templates in order to preserve source context for the
// MessageBuilder constructor.
//
// Construct messages by sending each provided argument directly to the output
// stream, with no additional formatting. Arguments must be supported by the
// "<<" output stream operator. Frequently, this means the appropriate header
// file needs to be included.

#define CUSTOM_LOG_MESSAGE(level, scope, timepoint, path, lineno, function) \
    core::logging::MessageBuilder::create_shared(                           \
        &core::logging::dispatcher,                                         \
        level,                                                              \
        scope,                                                              \
        timepoint,                                                          \
        path,                                                               \
        lineno,                                                             \
        function)

#define DEFAULT_LOG_MESSAGE(level) \
    CUSTOM_LOG_MESSAGE(            \
        level,                     \
        log_scope,                 \
        core::dt::Clock::now(),    \
        __builtin_FILE(),          \
        __builtin_LINE(),          \
        __builtin_FUNCTION())

#define LOG_MESSAGE(level, ...) \
    DEFAULT_LOG_MESSAGE(level)->add(__VA_ARGS__).dispatch()

#define log_trace(...)    LOG_MESSAGE(core::status::Level::TRACE, __VA_ARGS__)
#define log_debug(...)    LOG_MESSAGE(core::status::Level::DEBUG, __VA_ARGS__)
#define log_info(...)     LOG_MESSAGE(core::status::Level::INFO, __VA_ARGS__)
#define log_notice(...)   LOG_MESSAGE(core::status::Level::NOTICE, __VA_ARGS__)
#define log_warning(...)  LOG_MESSAGE(core::status::Level::WARNING, __VA_ARGS__)
#define log_error(...)    LOG_MESSAGE(core::status::Level::FAILED, __VA_ARGS__)
#define log_critical(...) LOG_MESSAGE(core::status::Level::CRITICAL, __VA_ARGS__)
#define log_fatal(...)    LOG_MESSAGE(core::status::Level::FATAL, __VA_ARGS__)

/// Construct messages from an format template and corresponding arguments.
/// Arguments must be supported by the "<<" output stream operator.  See
/// [string/format.h++](../string/format.h++) for details.

#define LOGF_MESSAGE(level, ...) \
    DEFAULT_LOG_MESSAGE(level)->format(__VA_ARGS__).dispatch()

#define logf_trace(...)    LOGF_MESSAGE(core::status::Level::TRACE, __VA_ARGS__)
#define logf_debug(...)    LOGF_MESSAGE(core::status::Level::DEBUG, __VA_ARGS__)
#define logf_info(...)     LOGF_MESSAGE(core::status::Level::INFO, __VA_ARGS__)
#define logf_notice(...)   LOGF_MESSAGE(core::status::Level::NOTICE, __VA_ARGS__)
#define logf_warning(...)  LOGF_MESSAGE(core::status::Level::WARNING, __VA_ARGS__)
#define logf_error(...)    LOGF_MESSAGE(core::status::Level::FAILED, __VA_ARGS__)
#define logf_critical(...) LOGF_MESSAGE(core::status::Level::CRITICAL, __VA_ARGS__)
#define logf_fatal(...)    LOGF_MESSAGE(core::status::Level::FATAL, __VA_ARGS__)

#ifndef NDEBUG
/// Evaluate a condition, exit with a fatal error if it fails.
#define assertf(cond, ...)                                                                \
    if (!(cond))                                                                          \
    {                                                                                     \
        std::cerr << "Assertion failed: " << core::str::format(__VA_ARGS__) << std::endl; \
        logf_fatal("Assertion failed: "s + __VA_ARGS__);                                  \
        std::abort();                                                                     \
    }
#else
#define assertf(cond, ...)
#endif
