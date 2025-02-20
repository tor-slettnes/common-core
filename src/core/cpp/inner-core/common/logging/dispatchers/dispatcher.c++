/// -*- c++ -*-
//==============================================================================
/// @file dispatcher.c++
/// @brief Distribute a captured item to available sinks
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dispatcher.h++"
#include "logging/sinks/factory.h++"
#include "status/exceptions.h++"

#include <iostream>

namespace core::logging
{
    void Dispatcher::initialize()
    {
        this->sinks_ = sink_registry.activate_sinks();
    }

    void Dispatcher::deinitialize()
    {
        for (const auto &[sink_id, sink] : this->sinks_)
        {
            sink->close();
        }
        this->sinks_.clear();
    }

    void Dispatcher::add_sinks(const SinkMap &sinks)
    {
        this->sinks_.insert(sinks.begin(), sinks.end());
    }

    Sink::ptr Dispatcher::add_sink(const Sink::ptr &sink)
    {
        if (sink)
        {
            return this->add_sink(sink->sink_id(), sink);
        }
        else
        {
            return {};
        }
    }

    Sink::ptr Dispatcher::add_sink(const SinkID &sink_id,
                                   const Sink::ptr &sink)
    {
        auto [it, inserted] = this->sinks_.emplace(sink_id, sink);
        return it->second;
    }

    bool Dispatcher::remove_sink(const SinkID &sink_id)
    {
        return this->sinks_.erase(sink_id);
    }

    bool Dispatcher::remove_sink(const Sink::ptr &sink)
    {
        for (auto it = this->sinks_.begin(); it != this->sinks_.end(); it++)
        {
            if (it->second == sink)
            {
                this->sinks_.erase(it);
                return true;
            }
        }
        return false;
    }

    Sink::ptr Dispatcher::get_sink(const SinkID &sink_id) const
    {
        return this->sinks_.get(sink_id);
    }

    const SinkMap &Dispatcher::sinks() const
    {
        return this->sinks_;
    }

    bool Dispatcher::is_applicable(const types::Loggable &item) const
    {
        for (const auto &[sink_id, sink] : this->sinks())
        {
            if (sink->is_applicable(item))
            {
                return true;
            }
        }
        return false;
    }

    void Dispatcher::submit(const types::Loggable::ptr &item)
    {
        std::lock_guard<std::mutex> lck(this->mtx_);
        for (const auto &[sink_id, sink] : this->sinks())
        {
            if (sink->is_applicable(*item))
            {
                sink->capture(item);
            }
        }
    }

    Dispatcher dispatcher;
}  // namespace core::logging
