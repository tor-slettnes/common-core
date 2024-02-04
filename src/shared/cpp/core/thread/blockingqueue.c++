/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.c++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "blockingqueue.h++"

namespace cc::types
{
    BlockingQueueBase::BlockingQueueBase(
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : maxsize_(maxsize),
          overflow_disposition_(overflow_disposition),
          cancelled(false)
    {
    }

    void BlockingQueueBase::cancel()
    {
        {
            std::lock_guard lock(this->mtx);
            this->cancelled = true;
        }
        this->cv.notify_all();
    }

    void BlockingQueueBase::uncancel()
    {
        std::lock_guard lock(this->mtx);
        this->cancelled = false;
    }

    bool BlockingQueueBase::pushable(std::unique_lock<std::mutex> *lock)
    {
        if ((this->maxsize_ == 0) || (this->size() < maxsize_))
        {
            return true;
        }
        else
        {
            switch (this->overflow_disposition_)
            {
            case OverflowDisposition::BLOCK:
                this->cv.wait(*lock, [&] {
                    return (this->size() < this->maxsize_) || this->cancelled;
                });
                return !this->cancelled;

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
}  // namespace cc::types
