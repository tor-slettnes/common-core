/// -*- c++ -*-
//==============================================================================
/// @file message.h++
/// @brief A log message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "scope.h++"
#include "types/loggable.h++"
#include "platform/host.h++"
#include "types/platform.h++"  // pid_t, ssize_t
#include "types/filesystem.h++"

namespace core::logging
{
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
        using This = Message;
        using Super = status::Event;

    public:
        using ptr = std::shared_ptr<Message>;

        inline static const std::string FIELD_HOST = "host";
        inline static const std::string FIELD_LOG_SCOPE = "log_scope";
        inline static const std::string FIELD_SOURCE_PATH = "source_path";
        inline static const std::string FIELD_SOURCE_LINE = "source_line";
        inline static const std::string FIELD_FUNCTION_NAME = "function_name";
        inline static const std::string FIELD_THREAD_ID = "thread_id";
        inline static const std::string FIELD_THREAD_NAME = "thread_name";
        inline static const std::string FIELD_TASK_NAME = "task_name";
        inline static const std::string CONTRACT = "text";

    public:
        /// @brief
        ///     Constructor.
        /// @param[in] text
        ///     Human readable text.
        /// @param[in] level
        ///     Severity level. Message will only be logged if its level is
        ///     equal to or higher than the threshold of the specified scope.
        /// @param[in] scope
        ///     Logging scope.
        /// @param[in] tp
        ///     Time point for the published message, if not now.
        /// @param[in] path
        ///     The source file from which the message originated.
        /// @param[in] lineno
        ///     The line number within the source file.
        /// @param[in] function
        ///     The function name in which the message originated.
        /// @param[in] thread_id
        ///     Numeric ID of the originating thread
        /// @param[in] thread_name
        ///     Name of the originating thread, if any.
        /// @param[in] task_name
        ///     Name of the originating task, if any.
        /// @param[in] host
        ///     Reporting host
        /// @param[in] origin
        ///     Originator entity, e.g. executable name
        /// @param[in] attributes
        ///     Key/value pairs associated with message

        Message(const std::string &text = "",
                status::Level level = status::Level::NONE,
                Scope::ptr scope = nullptr,
                const dt::TimePoint &tp = {},
                const fs::path &path = {},
                uint lineno = 0,
                const std::string &function = {},
                pid_t thread_id = 0,
                const std::string &thread_name = {},
                const std::string &task_name = {},
                const std::string &host = {},
                const std::string &origin = {},
                const types::KeyValueMap &attributes = {});

        // Copy constructor to ensure we obtain values from derived classes
        Message(const Message &other);

        Message &operator=(Message &&other) noexcept;
        Message &operator=(const Message &other) noexcept;
        bool operator==(const Message &other) const noexcept;

        /// Will this message be accepted by at least one available sink?
        virtual bool is_applicable() const noexcept;

        std::string contract_id() const noexcept override;

        virtual Scope::ptr scope() const noexcept;
        virtual std::string scopename_or(const std::string &fallback) const noexcept;
        virtual std::string scopename() const noexcept;

        virtual const fs::path &path() const noexcept;
        virtual uint lineno() const noexcept;
        virtual const std::string &function() const noexcept;
        virtual pid_t thread_id() const noexcept;
        virtual std::string thread_name() const noexcept;
        virtual std::string task_name() const noexcept;
        virtual std::string host() const noexcept;

    protected:
        std::string class_name() const noexcept override;

    public:
        static std::vector<std::string> message_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    protected:
        Scope::ptr scope_;
        fs::path path_;
        uint lineno_ = 0;
        std::string function_;
        pid_t thread_id_ = 0;
        std::string thread_name_;
        std::string task_name_;
        std::string host_;
    };

}  // namespace core::logging
