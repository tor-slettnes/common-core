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
    void MessageFormatter::set_include_context(bool include_context)
    {
        this->include_context_ = include_context;
    }

    bool MessageFormatter::include_context() const
    {
        return this->include_context_;
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
                       << msg->path().filename().string()
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
}  // namespace core::logging
