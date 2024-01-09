/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.c++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dispatcher.h++"
#include <iostream>

namespace cc::logging
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
        if (!this->workerthread_.joinable())
        {
            for (const Sink::Ref &sink : this->sinks())
            {
                sink->open();
            }
            this->workerthread_ = std::thread(&Dispatcher::worker, this);
        }
    }

    void Dispatcher::deinitialize()
    {
        if (this->workerthread_.joinable())
        {
            this->queue_.cancel();
            this->workerthread_.join();
            for (const Sink::Ref &sink : this->sinks_)
            {
                sink->close();
            }
        }
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
        this->queue_.put(item);
    }

    void Dispatcher::worker()
    {
        while (const std::optional<types::Loggable::Ref> &opt_item = this->queue_.get())
        {
            for (const Sink::Ref &sink : this->sinks())
            {
                const types::Loggable::Ref &item = *opt_item;
                if (sink->is_applicable(*item))
                {
                    sink->capture(item);
                }
            }
        }
    }

}  // namespace cc::logging
