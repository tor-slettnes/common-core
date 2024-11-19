/// -*- c++ -*-
//==============================================================================
/// @file message.h++
/// @brief A log message
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "scope.h++"
#include "status/event.h++"
#include "platform/host.h++"
#include "types/platform.h++"  // pid_t, ssize_t
#include "types/filesystem.h++"

namespace core::logging
{
    constexpr auto MESSAGE_FIELD_LOG_SCOPE = "log_scope";
    constexpr auto MESSAGE_FIELD_SOURCE_PATH = "source_path";
    constexpr auto MESSAGE_FIELD_SOURCE_LINE = "source_line";
    constexpr auto MESSAGE_FIELD_FUNCTION_NAME = "function_name";
    constexpr auto MESSAGE_FIELD_THREAD_ID = "thread_id";

    constexpr auto MESSAGE_CONTRACT = "text";

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

    public:
        /// @brief
        ///     Constructor.
        /// @param[in] text
        ///     Human readable text.
        /// @param[in] scope
        ///     Logging scope.
        /// @param[in] level
        ///     Severity level. Message will only be logged if its level is
        ///     equal to or higher than the threshold of the specified scope.
        /// @param[in] flow
        ///      Result on execution flow: NONE, CANCELLED, ABORTED
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
        /// @param[in] code
        ///     Numeric event code, unique within origin
        /// @param[in] symbol
        ///     Symbolic event code, unique within origin
        /// @param[in] attributes
        ///     Key/value pairs associated with message
        /// @param[in] contract_id
        ///     Log contract
        /// @param[in] host
        ///     Reporting host

        Message(const std::string &text = "",
                status::Level level = status::Level::NONE,
                Scope::ptr scope = nullptr,
                const dt::TimePoint &tp = {},
                const fs::path &path = {},
                uint lineno = 0,
                const std::string &function = {},
                pid_t thread_id = 0,
                status::Domain domain = status::Domain::APPLICATION,
                const std::string &origin = {},
                Code code = 0,
                const Symbol &symbol = {},
                const types::KeyValueMap &attributes = {},
                const ContractID &contract_id = MESSAGE_CONTRACT,
                const std::string &host = platform::host->get_host_name());

        // Copy constructor to ensure we obtain values from derived classes
        Message(const Message &other);

        Message &operator=(Message &&other) noexcept;
        Message &operator=(const Message &other) noexcept;

        std::string class_name() const noexcept override;
        // void to_stream(std::ostream &stream) const override;

        /// Will this message be accepted by at least one available sink?
        virtual bool is_applicable() const noexcept;
        virtual Scope::ptr scope() const noexcept;
        virtual std::string scopename_or(const std::string &fallback) const noexcept;
        virtual std::string scopename() const noexcept;
        virtual const fs::path &path() const noexcept;
        virtual uint lineno() const noexcept;
        virtual const std::string &function() const noexcept;
        virtual pid_t thread_id() const noexcept;

        static std::vector<std::string> message_fields() noexcept;
        std::vector<std::string> field_names() const noexcept override;
        types::Value get_field_as_value(const std::string &field_name) const override;

    protected:
        Scope::ptr scope_;
        fs::path path_;
        uint lineno_ = 0;
        std::string function_;
        pid_t thread_id_ = 0;
    };

}  // namespace core::logging
