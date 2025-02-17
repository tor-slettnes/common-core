/// -*- c++ -*-
//==============================================================================
/// @file builder.h++
/// @brief Construct a log message using `std::ostream` interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "message.h++"
#include "logging/dispatchers/dispatcher.h++"
#include "string/misc.h++"
#include "types/create-shared.h++"
#include "platform/process.h++"

namespace core::logging
{
    //==========================================================================
    /// @class MessageBuilder
    /// @brief
    ///    Log message constructed from `std::ostream` compatible types.
    ///
    /// This class builds a log message from `std::ostream` compatible elements
    /// (i.e., objects that support the `<<` operator).
    ///
    /// Elements are added to the stream if and only if it its severity level is
    /// equal to or higher than the logging threshold for the provided logs
    /// scope, _and_ if it is determined that the message will be accepted by at
    /// least one available `LogSink()` instance.  As such, to avoid the penalty
    /// of unneccessary string conversions, it is usually preferable to provide
    /// original objects rather than to preformat them as strings.  In this way,
    /// messages with low severity level (such as status::Level::TRACE) can be
    /// libreally added without affecting performance where higher logging
    /// thresholds are used.

    class MessageBuilder : public Message,
                           public std::ostringstream,
                           public types::enable_create_shared_from_this<MessageBuilder>
    {
        using Super = std::ostringstream;

    public:
        using ptr = std::shared_ptr<MessageBuilder>;

    protected:
        /// @brief
        ///     Constructor.
        /// @param[in] dispatcher
        ///     Log dispatcher that will receive and distribute the messsage
        /// @param[in] scope
        ///     Logging scope.
        /// @param[in] level
        ///     Severity level. Message will only be logged if its level is
        ///     equal to or higher than the threshold of the specified scope.
        /// @param[in] tp
        ///     Time point for the published message, if not now.
        /// @param[in] path
        ///     The source file from which the message originated.
        /// @param[in] lineno
        ///     The line number within the source file.
        /// @param[in] function
        ///     The function name in which the message originated.
        /// @param[in] thread_id
        ///     Identity of thread in which the message originated

        MessageBuilder(const std::shared_ptr<Dispatcher> &dispatcher,
                       status::Level level,
                       Scope::ptr scope,
                       const dt::TimePoint &tp,
                       const fs::path &path,
                       const uint &lineno,
                       const std::string &function,
                       pid_t thread_id = platform::process
                                             ? platform::process->thread_id()
                                             : 0);

    public:
        std::string text() const noexcept override;
        bool is_applicable() const noexcept override;
        void dispatch();

        template <class T>
        inline MessageBuilder &operator<<(const T &value)
        {
            if (this->is_applicable())
            {
                *static_cast<std::ostringstream *>(this) << value;
            }
            return *this;
        }

        template <class... Args>
        inline MessageBuilder &add(const Args &...args)
        {
            if (this->is_applicable())
                (*static_cast<std::ostringstream *>(this) << ... << args);
            return *this;
        }

        template <class... Args>
        inline MessageBuilder &format(const std::string &fmt, const Args &...args)
        {
            if (this->is_applicable())
            {
                str::format(*static_cast<std::ostringstream *>(this), fmt, args...);
            }
            return *this;
        }

    private:
        std::error_code path_error;
        std::shared_ptr<Dispatcher> dispatcher_;
        bool is_applicable_;
    };

}  // namespace core::logging
