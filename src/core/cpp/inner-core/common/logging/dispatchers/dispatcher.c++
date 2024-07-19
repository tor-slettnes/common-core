/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.c++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dispatcher.h++"
#include <iostream>

namespace core::logging
{
    Sink::Ref Dispatcher::add_sink(const Sink::Ref &sink)
    {
        auto [it, inserted] = this->sinks_.insert(sink);
        return *it;
    }

    bool Dispatcher::remove_sink(const Sink::Ref &sink)
    {
        return this->sinks_.erase(sink) > 0;
    }

    const std::set<Sink::Ref> &Dispatcher::sinks() const
    {
        return this->sinks_;
    }

    void Dispatcher::initialize()
    {
    }

    void Dispatcher::deinitialize()
    {
        this->sinks_.clear();
    }

    bool Dispatcher::is_applicable(const types::Loggable &item) const
    {
        for (const Sink::Ref &sink : this->sinks())
        {
            if (sink->is_applicable(item))
            {
                return true;
            }
        }
        return false;
    }

    void Dispatcher::submit(const types::Loggable::Ref &item)
    {
        for (const Sink::Ref &sink : this->sinks())
        {
            if (sink->is_applicable(*item))
            {
                sink->capture(item);
            }
        }
    }

}  // namespace core::logging
