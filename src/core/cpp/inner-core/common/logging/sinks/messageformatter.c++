/// -*- c++ -*-
//==============================================================================
/// @file messageformatter.c++
/// @brief Mix-in class to format messages for text-based log sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "messageformatter.h++"
#include "chrono/date-time.h++"

#include <sstream>

namespace core::logging
{
    MessageFormatter::MessageFormatter()
        : include_context_(DEFAULT_INCLUDE_CONTEXT)
    {
    }

    void MessageFormatter::load_message_format(const types::KeyValueMap &settings)
    {
        if (const types::Value &value = settings.get(SETTING_INCLUDE_CONTEXT))
        {
            this->set_include_context(value.as_bool());
        }
    }

    void MessageFormatter::set_all_include_context(bool include_context)
    {
        This::all_include_context_ = include_context;
    }

    void MessageFormatter::set_include_context(bool include_context)
    {
        this->include_context_ = include_context;
    }

    bool MessageFormatter::include_context() const
    {
        return this->all_include_context_.value_or(this->include_context_);
    }

    bool MessageFormatter::is_valid_message(const core::types::Loggable &item) const
    {
        return dynamic_cast<const Message *>(&item) != nullptr;
    }

    void MessageFormatter::send_preamble(std::ostream &stream,
                                         const status::Event::ptr &event) const
    {
        dt::tp_to_stream(stream, event->timepoint(), true, 3, "%F|%T");

        stream << "|"
               << std::setfill(' ')
               << std::setw(8)
               << event->level()
               << std::setw(0)
               << "|";

        if (this->include_context())
        {
            if (auto msg = std::dynamic_pointer_cast<Message>(event))
            {
                stream << msg->thread_id()
                       << "|"
                       << msg->scopename_or("(no scope)")
                       << "|"
                       << msg->path().string()
                       << ":"
                       << msg->lineno()
                       << ":"
                       << msg->function()
                       << "(): ";
            }
        }
    }

    std::string MessageFormatter::formatted(const status::Event::ptr &event) const
    {
        std::stringstream ss;
        this->send_preamble(ss, event);
        ss << event->text();
        return ss.str();
    }

    std::optional<bool> MessageFormatter::all_include_context_;
}  // namespace core::logging
