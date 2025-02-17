/// -*- c++ -*-
//==============================================================================
/// @file asynclogsink.c++
/// @brief Abstract base for asynchronous log sinks, derived from `LogSink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "asynclogsink.h++"

namespace core::logging
{
    void AsyncLogSink::open()
    {
        Super::open();
        if (!this->workerthread_.joinable())
        {
            this->queue.reopen();
            this->workerthread_ = std::thread(&This::worker, this);
        }
    }

    void AsyncLogSink::close()
    {
        if (this->workerthread_.joinable())
        {
            this->queue.close();
            this->workerthread_.join();
        }
        Super::close();
    }

    bool AsyncLogSink::capture(const types::Loggable::ptr &item)
    {
        if (auto event = std::dynamic_pointer_cast<status::Event>(item))
        {
            if (!this->queue.closed())
            {
                this->queue.put(event);
                return true;
            }
        }
        return false;
    }

    void AsyncLogSink::worker()
    {
        while (const std::optional<status::Event::ptr> &opt_event = this->queue.get())
        {
            this->try_capture_event(opt_event.value());
        }
    }
}  // namespace core::logging
