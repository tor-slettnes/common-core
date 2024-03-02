/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.c++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "blockingqueue.h++"

namespace core::types
{
    BlockingQueueBase::BlockingQueueBase(
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : maxsize_(maxsize),
          overflow_disposition_(overflow_disposition),
          closed_(false)
    {
    }

    void BlockingQueueBase::close()
    {
        {
            std::lock_guard lock(this->mtx);
            this->closed_ = true;
        }
        this->cv.notify_all();
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
            return true;
        }
        else
        {
            switch (this->overflow_disposition_)
            {
            case OverflowDisposition::BLOCK:
                this->cv.wait(*lock, [&] {
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
