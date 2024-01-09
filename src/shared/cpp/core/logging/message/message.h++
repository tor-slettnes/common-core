/// -*- c++ -*-
//==============================================================================
/// @file message.h++
/// @brief A log message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "scope.h++"
#include "status/event.h++"
#include "types/platform.h++"  // pid_t, ssize_t
#include "types/filesystem.h++"

namespace cc::logging
{
    constexpr auto MESSAGE_FIELD_LOG_SCOPE = "log_scope";
    constexpr auto MESSAGE_FIELD_SOURCE_PATH = "source_path";
    constexpr auto MESSAGE_FIELD_SOURCE_LINE = "source_line";
    constexpr auto MESSAGE_FIELD_FUNCTION_NAME = "function_name";
    constexpr auto MESSAGE_FIELD_THREAD_ID = "thread_id";

    //==========================================================================
    /// @class Message
    /// @brief Message data object, passed on to log sinks; see log().
    /// @note
    ///     This is not used directly when constructing log messages.
    ///     Instead, an instance of the derived class `MessageBuilder`
    ///     is created, which allows for `std::ostream` constructs such
    ///     as the `<<` operator.

    class Message : public status::Event
    {
        using Super = status::Event;

    public:
        using Ref = std::shared_ptr<Message>;

    public:
        /// @brief
        ///     Constructor.
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
        ///     Thread ID where where the message was generated
        /// @param[in] origin
        ///     Originator entity, e.g. executable name
        /// @param[in] attributes
        ///     Key/value pairs associated with message
        /// @param[in] text
        ///     Human readable text.

        Message(Scope::Ref scope,
                status::Level level,
                const dt::TimePoint &tp,
                const fs::path &path,
                uint lineno,
                const std::string &function,
                pid_t thread_id,
                const std::string &origin,
                const types::KeyValueMap &attributes,
                const std::string &text);

        // Copy constructor to ensure we obtain values from derived classes
        Message(const Message &other);

        Message &operator=(const Message &other) noexcept;

        void to_stream(std::ostream &stream) const override
        {
            stream << this->text();
        }

        /// Will this message be accepted by at least one available sink?
        inline virtual bool is_applicable() const noexcept
        {
            if (const Scope::Ref &scope = this->scope())
            {
                return scope->is_applicable(this->level());
            }
            else
            {
                return false;
            }
        }

        virtual inline Scope::Ref scope() const noexcept
        {
            return this->scope_;
        }

        virtual inline std::string scopename_or(const std::string &fallback) const noexcept
        {
            return this->scope() ? this->scope()->name : fallback;
        }

        virtual inline std::string scopename() const noexcept
        {
            return this->scopename_or("");
        }

        virtual inline const fs::path &path() const noexcept
        {
            return this->path_;
        }

        virtual inline uint lineno() const noexcept
        {
            return this->lineno_;
        }

        virtual inline const std::string &function() const noexcept
        {
            return this->function_;
        }

        virtual inline pid_t thread_id() const noexcept
        {
            return this->thread_id_;
        }

    protected:
        void populate_fields(types::TaggedValueList *values) const noexcept override;

    protected:
        Scope::Ref scope_;
        fs::path path_;
        uint lineno_;
        std::string function_;
        pid_t thread_id_;
    };

}  // namespace cc::logging
