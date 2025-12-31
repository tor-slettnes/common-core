/// -*- c++ -*-
//==============================================================================
/// @file messagesink.c++
/// @brief Log sink for text messages
/// @author Tor Slettnes
//==============================================================================

#include "messagesink.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::logging
{
    const std::string SETTING_INCLUDE_CONTEXT = "include context";
    const bool DEFAULT_INCLUDE_CONTEXT = true;

    const std::string SETTING_INCLUDE_SOURCE = "include source location";
    const bool DEFAULT_INCLUDE_SOURCE = false;

    //--------------------------------------------------------------------------
    // MessageSink

    MessageSink::MessageSink(const std::string &sink_id)
        : Sink(sink_id, Message::CONTRACT),
          include_context_(DEFAULT_INCLUDE_CONTEXT),
          include_source_(DEFAULT_INCLUDE_SOURCE)
    {
    }

    void MessageSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);
        this->load_message_settings(settings);
    }

    void MessageSink::load_message_settings(const types::KeyValueMap &settings)
    {
        if (const types::Value &value = settings.get(SETTING_INCLUDE_CONTEXT))
        {
            this->set_include_context(value.as_bool());
        }

        if (const types::Value &value = settings.get(SETTING_INCLUDE_SOURCE))
        {
            this->set_include_source_location(value.as_bool());
        }
    }

    void MessageSink::set_include_context(bool include_context)
    {
        this->include_context_ = include_context;
    }

    bool MessageSink::include_context() const
    {
        return this->include_source_location() ||
            This::all_include_context_.value_or(this->include_context_);
    }

    void MessageSink::set_all_include_context(bool include_context)
    {
        This::all_include_context_ = include_context;
    }

    void MessageSink::set_include_source_location(bool include_source)
    {
        this->include_source_ = include_source;
    }

    bool MessageSink::include_source_location() const
    {
        return This::all_include_source_.value_or(this->include_source_);
    }

    void MessageSink::set_all_include_source_location(bool include_source)
    {
        This::all_include_source_ = include_source;
    }

    bool MessageSink::is_applicable(const types::Loggable &item) const
    {
        if (auto event = dynamic_cast<const Message *>(&item))
        {
            return Super::is_applicable(item);
        }
        else
        {
            return false;
        }
    }

    bool MessageSink::handle_item(const types::Loggable::ptr &item)
    {
        if (auto message = std::dynamic_pointer_cast<Message>(item))
        {
            return this->handle_message(message);
        }
        else
        {
            return false;
        }
    }

    void MessageSink::send_preamble(std::ostream &stream,
                                    const Message::ptr &message) const
    {
        dt::tp_to_stream(stream, message->timepoint(), true, 3, "%F|%T");

        stream << "|"
               << std::right
               << std::setfill(' ')
               << std::setw(8)
               << message->level()
               << std::left
               << "|";

        if (this->include_context())
        {
            if (!message->origin().empty())
            {
                stream << std::setw(12)
                       << message->origin()
                       << "|";
            }

            stream << std::setw(12)
                   << message->scopename_or("(no scope)")
                   << "|";

            if (!message->task_name().empty())
            {
                stream << std::setw(8)
                       << message->task_name()
                       << "|";
            }
            else if (!message->thread_name().empty())
            {
                stream << std::setw(8)
                       << message->thread_name()
                       << "|";
            }
            else if (message->thread_id())
            {
                stream << std::setw(8)
                       << std::right
                       << message->thread_id()
                       << std::left
                       << "|";
            }
        }


        if (this->include_source_location())
        {
            if (!message->path().empty())
            {
                stream << message->path().string()
                       << ":"
                       << message->lineno()
                       << ":"
                       << message->function()
                       << "(): ";
            }
        }
    }

    std::string MessageSink::formatted(const Message::ptr &message) const
    {
        std::stringstream ss;
        this->send_preamble(ss, message);
        ss << message->text();
        return ss.str();
    }

    std::optional<bool> MessageSink::all_include_context_;
    std::optional<bool> MessageSink::all_include_source_;
}  // namespace core::logging
