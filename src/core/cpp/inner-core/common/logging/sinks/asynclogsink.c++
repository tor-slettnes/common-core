/// -*- c++ -*-
//==============================================================================
/// @file asynclogsink.c++
/// @brief Abstract base for asynchronous log sinks, derived from `LogSink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "asynclogsink.h++"

namespace core::logging
{
    AsyncLogSink::AsyncLogSink()
    {
    }

    void AsyncLogSink::open()
    {
        Super::open();
        if (!this->workerthread_.joinable())
        {
            this->queue_.reopen();
            this->workerthread_ = std::thread(&This::worker, this);
        }
    }

    void AsyncLogSink::close()
    {
        if (this->workerthread_.joinable())
        {
            this->queue_.close();
            this->workerthread_.join();
        }
        Super::close();
    }

    bool AsyncLogSink::capture(const types::Loggable::ptr &item)
    {
        if (auto event = std::dynamic_pointer_cast<status::Event>(item))
        {
            this->queue_.put(event);
            return true;
        }
        else
        {
            return false;
        }
    }

    void AsyncLogSink::worker()
    {
        while (const std::optional<status::Event::ptr> &opt_event = this->queue_.get())
        {
            this->capture_event(opt_event.value());
        }
    }
}  // namespace core::logging
