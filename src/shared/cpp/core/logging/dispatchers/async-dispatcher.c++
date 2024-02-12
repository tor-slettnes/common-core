/// -*- c++ -*-
//==============================================================================
/// @file async-dispatcher.c++
/// @brief Distribute a captured item to available sinks - asynchronous flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "async-dispatcher.h++"

namespace shared::logging
{
    void AsyncDispatcher::initialize()
    {
        Dispatcher::initialize();
        if (!this->workerthread_.joinable())
        {
            for (const Sink::Ref &sink : this->sinks())
            {
                sink->open();
            }
            this->workerthread_ = std::thread(&AsyncDispatcher::worker, this);
        }
    }

    void AsyncDispatcher::deinitialize()
    {
        if (this->workerthread_.joinable())
        {
            this->queue_.cancel();
            this->workerthread_.join();
            for (const Sink::Ref &sink : this->sinks())
            {
                sink->close();
            }
        }
        Dispatcher::deinitialize();
    }

    void AsyncDispatcher::submit(const types::Loggable::Ref &item)
    {
        this->queue_.put(item);
    }

    void AsyncDispatcher::worker()
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
}  // namespace shared::logging
