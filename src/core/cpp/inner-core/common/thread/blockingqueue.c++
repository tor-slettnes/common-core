/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.c++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "blockingqueue.h++"
#include "platform/init.h++"

namespace core::types
{
    BlockingQueueBase::BlockingQueueBase(
        std::size_t maxsize,
        OverflowDisposition overflow_disposition,
        bool close_on_shutdown)
        : maxsize_(maxsize),
          overflow_disposition_(overflow_disposition),
          closed_(false)
    {
        if (close_on_shutdown)
        {
            this->shutdown_handle_ = platform::signal_shutdown.connect([&] {
                this->close();
            });
        }
    }

    BlockingQueueBase::~BlockingQueueBase()
    {
        if (!this->shutdown_handle_.empty())
        {
            platform::signal_shutdown.disconnect(this->shutdown_handle_);
        }
    }

    void BlockingQueueBase::close()
    {
        {
            std::lock_guard lock(this->mtx);
            this->closed_ = true;
        }
        this->space_available.notify_all();
        this->item_available.notify_all();
    }

    void BlockingQueueBase::reopen()
    {
        std::lock_guard lock(this->mtx);
        this->closed_ = false;
    }

    bool BlockingQueueBase::closed() const
    {
        return this->closed_;
    }

    bool BlockingQueueBase::pushable(std::unique_lock<std::mutex> *lock)
    {
        if ((this->maxsize_ == 0) || (this->size() < this->maxsize_))
        {
            return !this->closed_;
        }
        else
        {
            switch (this->overflow_disposition_)
            {
            case OverflowDisposition::BLOCK:
                this->space_available.wait(*lock, [&] {
                    return (this->size() < this->maxsize_) || this->closed_;
                });
                return !this->closed_;

            case OverflowDisposition::DISCARD_ITEM:
                return false;

            case OverflowDisposition::DISCARD_OLDEST:
                this->discard_oldest();
                return true;

            default:
                return false;
            }
        }
    }
}  // namespace core::types
