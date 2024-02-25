/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief Abstract base for log sinks, derived from `capture::Sink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logsink.h++"

namespace shared::logging
{
    //==========================================================================
    // LogSink

    LogSink::LogSink()
        : threshold_(status::Level::TRACE)
    {
    }

    bool LogSink::is_applicable(const types::Loggable &item) const
    {
        if (auto event = dynamic_cast<const status::Event *>(&item))
        {
            return (this->threshold() != status::Level::NONE) &&
                   (event->level() >= this->threshold());
        }
        else
        {
            return false;
        }
    }

    bool LogSink::capture(const types::Loggable::Ref &item)
    {
        if (auto event = std::dynamic_pointer_cast<status::Event>(item))
        {
            this->capture_event(event);
            return true;
        }
        return false;
    }

    void LogSink::set_threshold(status::Level threshold)
    {
        this->threshold_ = threshold;
    }

    status::Level LogSink::threshold() const
    {
        return this->threshold_;
    }

}  // namespace shared::logging
