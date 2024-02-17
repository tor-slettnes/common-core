/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.c++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "blockingqueue.h++"

namespace shared::types
{
    BlockingQueueBase::BlockingQueueBase(
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : maxsize_(maxsize),
          overflow_disposition_(overflow_disposition),
          closed(false)
    {
    }

    void BlockingQueueBase::close()
    {
        {
            std::lock_guard lock(this->mtx);
            this->closed = true;
        }
        this->cv.notify_all();
    }

    void BlockingQueueBase::reopen()
    {
        std::lock_guard lock(this->mtx);
        this->closed = false;
    }

    bool BlockingQueueBase::pushable(std::unique_lock<std::mutex> *lock)
    {
        if ((this->maxsize_ == 0) || (this->size() < this->maxsize_))
        {
            return true;
        }
        else
        {
            switch (this->overflow_disposition_)
            {
            case OverflowDisposition::BLOCK:
                this->cv.wait(*lock, [&] {
                    return (this->size() < this->maxsize_) || this->closed;
                });
                return !this->closed;

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
}  // namespace shared::types
