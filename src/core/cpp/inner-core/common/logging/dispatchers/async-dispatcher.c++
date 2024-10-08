/// -*- c++ -*-
//==============================================================================
/// @file async-dispatcher.c++
/// @brief Distribute a captured item to available sinks - asynchronous flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "async-dispatcher.h++"

namespace core::logging
{
    void AsyncDispatcher::initialize()
    {
        Dispatcher::initialize();
        if (!this->workerthread_.joinable())
        {
            for (const Sink::ptr &sink : this->sinks())
            {
                sink->open();
            }
            this->queue_.reopen();
            this->workerthread_ = std::thread(&AsyncDispatcher::worker, this);
        }
    }

    void AsyncDispatcher::deinitialize()
    {
        if (this->workerthread_.joinable())
        {
            this->queue_.close();
            this->workerthread_.join();
            for (const Sink::ptr &sink : this->sinks())
            {
                sink->close();
            }
        }
        Dispatcher::deinitialize();
    }

    void AsyncDispatcher::submit(const types::Loggable::ptr &item)
    {
        this->queue_.put(item);
    }

    void AsyncDispatcher::worker()
    {
        while (const std::optional<types::Loggable::ptr> &opt_item = this->queue_.get())
        {
            for (const Sink::ptr &sink : this->sinks())
            {
                const types::Loggable::ptr &item = *opt_item;
                if (sink->is_applicable(*item))
                {
                    sink->capture(item);
                }
            }
        }
    }
}  // namespace core::logging
