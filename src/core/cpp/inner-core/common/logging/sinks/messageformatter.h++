/// -*- c++ -*-
//==============================================================================
/// @file messageformatter.h++
/// @brief Mix-in class to format messages for text-based log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "../message/message.h++"

#include <string>
#include <memory>
#include <ostream>

namespace core::logging
{
    const std::string SETTING_INCLUDE_CONTEXT = "include context";
    const bool DEFAULT_INCLUDE_CONTEXT = false;

    //==========================================================================
    /// @class MessageFormatter
    /// @brief Mix-in class for traits specific to text-based log sinks

    class MessageFormatter
    {
        using This = MessageFormatter;

    protected:
        MessageFormatter();

    public:
        static void set_all_include_context(bool include_context);
        void set_include_context(bool include_context);
        bool include_context() const;
        bool is_valid_message(const core::types::Loggable &item) const;

    protected:
        void load_message_format(const types::KeyValueMap &settings);

        void send_preamble(std::ostream &stream,
                           const status::Event::ptr &event) const;

        std::string formatted(const status::Event::ptr &event) const;

    private:
        static std::optional<bool> all_include_context_;
        bool include_context_;
    };
}  // namespace core::logging
