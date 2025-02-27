/// -*- c++ -*-
//==============================================================================
/// @file capture.c++
/// @brief Capture loggable items, synchronously or asynchronously
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "capture.h++"

namespace core::logging
{
    //--------------------------------------------------------------------------
    // @class Capture

    Capture::Capture(const Handler handler)
        : handler(handler)
    {
    }

    //--------------------------------------------------------------------------
    // @class PassthroughCapture

    bool PassthroughCapture::operator()(const types::Loggable::ptr &item)
    {
        return this->handler(item);
    }

    //--------------------------------------------------------------------------
    // @class AsyncCapture

    void AsyncCapture::start()
    {
        if (!this->workerthread_.joinable())
        {
            this->queue.reopen();
            this->workerthread_ = std::thread(&This::worker, this);
        }
    }

    void AsyncCapture::stop()
    {
        this->queue.close();
        if (this->workerthread_.joinable())
        {
            this->workerthread_.join();
        }
    }

    bool AsyncCapture::operator()(const types::Loggable::ptr &item)
    {
        if (!this->queue.closed())
        {
            this->queue.put(item);
            return true;
        }
        else
        {
            return false;
        }
    }

    void AsyncCapture::worker()
    {
        while (const std::optional<types::Loggable::ptr> &opt_item = this->queue.get())
        {
            this->handler(opt_item.value());
        }
    }
}  // namespace core::logging
