/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.h++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <chrono>
#include <optional>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <algorithm>

namespace cc::types
{
    /// @class BlockingQueue
    /// @brief Implements a Condition() wrapper around std::queue
    /// @tparam T
    ///    Data type

    template <class T>
    class BlockingQueue
    {
    public:
        /// @brief Constructor
        /// @param[in] maxsize
        ///    Maximum queue size, after which the oldest element is removed
        ///    from the queue. Zero (the default) means unlimited size.

        BlockingQueue(unsigned int maxsize = 0)
            : maxsize(maxsize),
              cancelled(false)
        {
        }

        /// @brief
        ///     Add an item to the end of the queue
        /// @param[in] value
        ///     Value to copy to the end of the queue. \sa void put(T && value).
        ///
        /// Add a new item at the end of the queue.  This unblocks exactly one
        /// pending or future `.get()` call.
        ///
        /// If the queue is full (i.e. if there is a maximum queue size and it
        /// has been reached) then the oldest item is removed from the front of
        /// the queue.

        inline void put(const T &value)
        {
            {
                std::lock_guard lock(this->mtx);
                if ((this->maxsize > 0) && (this->size() >= maxsize))
                    this->queue.pop();
                this->queue.push(value);
                this->cancelled = false;
            }
            this->cv.notify_one();
        }

        /// @brief
        ///     Add an item to the queue
        /// @param[in] value
        ///     Value which is moved to the end of the queue.
        ///
        /// Add a new item at the end of the queue.  This unblocks exactly one
        /// pending or future `.get()` call.
        ///
        /// If the queue is full (i.e. if there is a maximum queue size and it
        /// has been reached) then the oldest item is removed from the front of
        /// the queue.

        inline void put(T &&value)
        {
            {
                std::lock_guard lock(this->mtx);
                if ((this->maxsize > 0) && (this->size() >= maxsize))
                    this->queue.pop();
                this->queue.push(std::move(value));
                this->cancelled = false;
            }
            this->cv.notify_all();
        }

        /// @brief
        ///     Cancel any pending `get()` calls.
        ///
        /// Any pending `get()` calls will return immediately with an empty
        /// value.  Future calls are also cancelled, until either a new item is
        /// placed in the queue with `put()`, or the queue is explicitly resumed
        /// with `uncancel()`.

        inline void cancel()
        {
            {
                std::lock_guard lock(this->mtx);
                this->cancelled = true;
            }
            this->cv.notify_all();
        }

        /// @brief
        ///     Resume the queue after `cancel()`
        ///
        /// Future `get()` calls will again block.

        inline void uncancel()
        {
            std::lock_guard lock(this->mtx);
            this->cancelled = false;
        }

        /// @brief
        ///     Get an item from the queue
        ///
        /// @return
        ///     An optional container with the value from the beinning of the queue
        ///     if available, otherwise empty.
        ///
        /// @note
        ///     The call blocks until an item is available or until cancelled.
        ///     \sa cancel().

        inline std::optional<T> get()
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            this->cv.wait(lock, [&] {
                return this->queue.size() || this->cancelled;
            });

            if (this->queue.size())
            {
                T value = std::move(this->queue.front());
                this->queue.pop();
                return value;
            }
            else
            {
                return {};
            }
        }

        /// @brief
        ///     Get an item from the queue with a fixed deadline.
        ///
        /// @param[in] deadline
        ///     Monotonic time point after which to return an empty value.
        ///
        /// @return
        ///     An optional container with the value from the beinning of the queue
        ///     if available within the specified deadline, otherwise empty.
        ///
        /// @note
        ///     The call blocks until an item is available, until cancelled, or
        ///     until the specified deadline has arrived.

        template <class Clock, class Duration>
        inline std::optional<T> get(const std::chrono::time_point<Clock, Duration> &deadline)
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            bool received = this->cv.wait_until(lock, deadline, [&] {
                return this->queue.size() || this->cancelled;
            });

            if (received)
            {
                T value = std::move(this->queue.front());
                this->queue.pop();
                return value;
            }
            else
            {
                return {};
            }
        }

        /// @brief
        ///     Get an item from the queue with a timeout
        ///
        /// @param[in] timeout
        ///     Duration after which to return an empty value.
        ///
        /// @return
        ///     An optional container with the value from the beinning of the queue
        ///     if available, otherwise empty.
        ///
        /// @note
        ///     The call blocks until an item is available, until cancelled, or
        ///     until the specified deadline has arrived.

        template <class Rep, class Period>
        inline std::optional<T> get(const std::chrono::duration<Rep, Period> &timeout)
        {
            return this->get(
                std::chrono::steady_clock::now() +
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout));
        }

        /// @brief
        ///     Obtain the size of the queue
        ///
        /// @return
        ///     Number of elements in the queue

        inline std::size_t size()
        {
            return this->queue.size();
        }

        /// @brief
        ///     Indicate whether the queue is empty (and a call to `.get()` would block)
        ///
        /// @return
        ///     Boolean indication of whether the queue is empty.

        inline bool empty()
        {
            return this->queue.empty();
        }

    private:
        unsigned int maxsize;
        bool cancelled;
        std::condition_variable cv;
        std::mutex mtx;
        std::queue<T> queue;
    };

}  // namespace cc::types
