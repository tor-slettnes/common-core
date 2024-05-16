/// -*- c++ -*-
//==============================================================================
/// @file binaryevent.h++
/// @brief Binary status
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <functional>

namespace core::types
{
    /// \class Event
    /// \brief Wait for / manage binary event
    ///
    /// This is one of the simplest mechanisms for communicating between threads:
    /// One thread signals an event and others wait for it. MOdeled after:
    ///     https://docs.python.org/3/library/threading.html#event-objects

    class BinaryEvent
    {
    public:
        /// \brief Constructor
        /// \param[in] initial_value
        ///     Whether the switch is intially set. Until explicitly cleared,
        ///     any `wait()` calls will then return immediately.
        BinaryEvent(bool initial_value = false);

        /// \brief
        ///     Set the internal flag. If the specified value is `true` (default),
        ///     unblock anyone waiting for this event.
        /// \param[in] value
        ///     Flag value
        void set(bool value = true);

        /// \brief
        ///     Clear the internal flag. Identical to `set(false)`.
        void clear();

        /// \brief
        ///     Unblock anyone waiting for this event, but do not modify the
        ///     internal flag.
        void cancel();

        /// \brief
        ///     Nonblocking query of whether the event is set.
        /// \return
        ///     Internal flag.
        bool is_set() const;

        /// \brief
        ///     Wait indefinitely for the event to be set.
        void wait();

        /// \brief
        ///     Wait for the event to be set or until the specified deadline has expired.
        /// \param[in] deadline
        ///     Timepoint after which to return, even if the event has not yet been set.
        /// \return
        ///     `true` if the flag was set before the deadline expired, `false` otherwise.
        template <class Clock, class Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration> &deadline)
        {
            std::unique_lock<std::mutex> lock(this->mtx_);
            return this->cv_.wait_until(lock, deadline, std::bind(&BinaryEvent::is_set, this));
        }

        /// \brief
        ///     Wait for the event to be set or until the specified time has elapsed
        /// \param[in] timeout
        ///     Duration after which to return, even if the event has not yet been set
        /// \return
        ///     `true` if the flag was set before the timeout elapsed, `false` otherwise.
        template <class Rep, class Period = std::ratio<1>>
        bool wait_for(const std::chrono::duration<Rep, Period> &timeout)
        {
            return this->wait_until(
                std::chrono::steady_clock::now() +
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout));
        }

    private:
        std::condition_variable cv_;
        std::mutex mtx_;
        bool value_;
        bool ready_;
    };

}  // namespace core::types
