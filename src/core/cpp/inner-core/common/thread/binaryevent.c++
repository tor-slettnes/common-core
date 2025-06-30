/// -*- c++ -*-
//==============================================================================
/// @file binaryevent.c++
/// @brief Binary status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "binaryevent.h++"

namespace core::types
{
    //==========================================================================
    // BinaryEvent

    BinaryEvent::BinaryEvent(bool initial_value)
        : value_(initial_value),
          ready_(initial_value)
    {
    }

    void BinaryEvent::set(bool value)
    {
        {
            std::scoped_lock lock(this->event_mtx_);
            this->value_ = this->ready_ = value;
        }
        this->cv_.notify_all();
    }

    void BinaryEvent::clear()
    {
        this->set(false);
    }

    void BinaryEvent::cancel()
    {
        {
            std::scoped_lock lock(this->event_mtx_);
            this->ready_ = true;
        }
        this->cv_.notify_all();
    }

    bool BinaryEvent::is_set() const
    {
        return this->value_;
    }

    void BinaryEvent::wait()
    {
        std::unique_lock<std::mutex> lock(this->event_mtx_);
        this->ready_ = this->value_;
        this->cv_.wait(lock, [&] {
            return this->ready_;
        });
    }

}  // namespace core::types
