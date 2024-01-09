/// -*- c++ -*-
//==============================================================================
/// @file messagesink.c++
/// @brief Abstract base for log sinks that accept simple text strings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "messagesink.h++"

#include <sstream>

namespace cc::logging
{
    void MessageSink::set_include_source(bool include_source)
    {
        this->include_source_ = include_source;
    }

    bool MessageSink::include_source() const
    {
        return this->include_source_;
    }

    bool MessageSink::is_applicable(const types::Loggable &item) const
    {
        if (dynamic_cast<const Message *>(&item))
        {
            return Super::is_applicable(item);
        }
        else
        {
            return false;
        }
    }

    void MessageSink::capture_event(const status::Event::Ref &event)
    {
        if (auto msg = std::dynamic_pointer_cast<Message>(event))
        {
            this->capture_message(msg);
        }
    }

    void MessageSink::send_preamble(std::ostream &stream,
                                    const Message::Ref &msg,
                                    const std::string &suffix) const
    {
        if (this->include_source())
        {
            stream << msg->scopename_or("(no scope)")
                   << "|"
                   << msg->thread_id()
                   << "|"
                   << msg->path().filename().string()
                   << ":"
                   << msg->lineno()
                   << ":"
                   << msg->function()
                   << "()"
                   << suffix;
        }
    }

    std::string MessageSink::preamble(const Message::Ref &msg,
                                      const std::string &suffix) const
    {
        std::stringstream ss;
        this->send_preamble(ss, msg, suffix);
        return ss.str();
    }
}  // namespace cc::logging
