/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.h++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#pragma once
#include <chrono>
#include <optional>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <algorithm>

namespace core::types
{
    //==========================================================================
    /// @enum OverlowDisposition
    /// @brief How to handle `.put()` invocatinos into a full queue
    enum class OverflowDisposition
    {
        BLOCK,
        DISCARD_ITEM,
        DISCARD_OLDEST,
    };

    //==========================================================================
    /// @class BlockingQueueBase
    /// @brief Untyped base for BlockingQueue

    class BlockingQueueBase
    {
    protected:
        BlockingQueueBase(
            unsigned int maxsize,
            OverflowDisposition overflow_disposition);


    public:
        /// @brief
        ///     Close the queue.
        ///
        /// Any pending `get()` calls will return immediately with an empty
        /// value.  Future calls are also closed, until either a new item is
        /// placed in the queue with `put()`, or the queue is explicitly resumed
        /// with `reopen()`.
        void close();

        /// @brief
        ///     Resume the queue after `end()`
        ///
        /// Future `get()` calls will again block.
        void reopen();

        /// @brief
        ///     Obtain the size of the queue
        ///
        /// @return
        ///     Number of elements in the queue
        virtual std::size_t size() = 0;

        /// @brief
        ///     Indicate whether the queue is empty (and a call to `.get()` would block)
        ///
        /// @return
        ///     Boolean indication of whether the queue is empty.
        virtual bool empty() = 0;


        /// @brief
        ///     Remove the oldest item from the queue
        virtual void discard_oldest() = 0;


    protected:
        /// @brief
        ///     Indicate whether an item can be pushed into the queue.
        ///
        /// @return
        ///     Boolean indication of whether an item can be pushed into the queue.
        ///
        /// @note
        ///     May block if `overflow_disposition == BLOCK`.
        bool pushable(std::unique_lock<std::mutex> *lock);

    protected:
        const unsigned int maxsize_;
        const OverflowDisposition overflow_disposition_;

    protected:
        bool closed;
        std::condition_variable cv;
        std::mutex mtx;
    };

    //==========================================================================
    /// @class BlockingQueue
    /// @brief Implements a Condition() wrapper around std::queue
    /// @tparam T
    ///    Data type

    template <class T>
    class BlockingQueue : public BlockingQueueBase
    {
    public:
        /// @brief Constructor
        /// @param[in] maxsize
        ///    Maximum queue size, after which the oldest element is removed
        ///    from the queue. Zero (the default) means unlimited size.
        /// @param[in] full_

        BlockingQueue(
            unsigned int maxsize = 0,
            OverflowDisposition overflow_disposition = OverflowDisposition::DISCARD_OLDEST)
            : BlockingQueueBase(maxsize, overflow_disposition)
        {
        }

        inline std::size_t size() override
        {
            return this->queue.size();
        }

        inline bool empty() override
        {
            return this->queue.empty();
        }

        inline void discard_oldest() override
        {
            this->queue.pop();
        }

    public:
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
                std::unique_lock<std::mutex> lock(this->mtx);
                this->closed = false;
                if (this->pushable(&lock))
                {
                    this->queue.push(value);
                }
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
                std::unique_lock<std::mutex> lock(this->mtx);
                this->closed = false;
                if (this->pushable(&lock))
                {
                    this->queue.push(std::move(value));
                }
            }
            this->cv.notify_all();
        }

        /// @brief
        ///     Get an item from the queue
        ///
        /// @return
        ///     An optional container with the value from the beinning of the queue
        ///     if available, otherwise empty.
        ///
        /// @note
        ///     The call blocks until an item is available or until closed.
        ///     \sa end().

        inline std::optional<T> get()
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            this->cv.wait(lock, [&] {
                return this->queue.size() || this->closed;
            });

            if (this->queue.size())
            {
                T value = std::move(this->queue.front());
                this->discard_oldest();
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
        ///     The call blocks until an item is available, until closed, or
        ///     until the specified deadline has arrived.

        template <class Clock, class Duration>
        inline std::optional<T> get(const std::chrono::time_point<Clock, Duration> &deadline)
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            bool received = this->cv.wait_until(lock, deadline, [&] {
                return this->queue.size() || this->closed;
            });

            if (received)
            {
                T value = std::move(this->queue.front());
                this->discard_oldest();
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
        ///     The call blocks until an item is available, until closed, or
        ///     until the specified deadline has arrived.

        template <class Rep, class Period>
        inline std::optional<T> get(const std::chrono::duration<Rep, Period> &timeout)
        {
            return this->get(
                std::chrono::steady_clock::now() +
                std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout));
        }



    private:
        std::queue<T> queue;
    };

}  // namespace core::types
