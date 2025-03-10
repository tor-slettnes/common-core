/// -*- c++ -*-
//==============================================================================
/// @file blockingqueue.h++
/// @brief std::queue wrapper with blocking receiver
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/getter.h++"
#include "platform/init.h++"

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
    /// @class BlockingQueueBase
    /// @brief Untyped base for BlockingQueue

    class BlockingQueueBase
    {
    public:
        //==========================================================================
        /// @enum OverflowDisposition
        /// @brief How to handle `.put()` invocatinos into a full queue
        enum class OverflowDisposition
        {
            BLOCK,          // Block the calling thread until space is available
            DISCARD_ITEM,   // Discard the new item instead of placing into queue
            DISCARD_OLDEST, // Discard the oldest item in the queue to make space
        };

    protected:
        /// @brief
        ///     Constructor
        /// @param[in] maxsize
        ///     Maximum queue size.
        /// @param[in] overflow_disposition
        ///     How to handle `put()` requests into a full queue.
        /// @param[in] close_on_shutdown
        ///     Close this queue on program shutdown (to prevent hang)

        BlockingQueueBase(
            std::size_t maxsize,
            OverflowDisposition overflow_disposition);

    public:
        /// @brief
        ///     Clear the queue.
        virtual void clear();


        /// @brief
        ///     Close the queue.
        ///
        /// Any pending `get()` calls will return immediately with an empty
        /// value.  Future calls are also closed, until either a new item is
        /// placed in the queue with `put()`, or the queue is explicitly resumed
        /// with `reopen()`.
        virtual void close();

        /// @brief
        ///     Resume the queue after `end()`
        ///
        /// Future `get()` calls will again block.
        virtual void reopen();

        /// @brief
        ///     Indicate whether this queue has been closed
        bool closed() const;

        /// @brief
        ///     Obtain the size of the queue
        ///
        /// @return
        ///     Number of elements in the queue
        virtual std::size_t size() const = 0;

        /// @brief
        ///     Indicate whether the queue is empty (and a call to `.get()` would block)
        ///
        /// @return
        ///     Boolean indication of whether the queue is empty.
        virtual bool empty() const = 0;

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
        const std::size_t maxsize_;
        const OverflowDisposition overflow_disposition_;
        bool closed_;
        std::string shutdown_handle_;
        std::condition_variable item_available, space_available;
        std::mutex mtx;
    };

    //==========================================================================
    /// @class BlockingQueue
    /// @brief Implements a Condition() wrapper around std::queue
    /// @tparam T
    ///    Data type

    template <class T>
    class BlockingQueue : public BlockingQueueBase,
                          public Getter<T>
    {
    public:
        /// @brief Constructor
        /// @param[in] maxsize
        ///    Maximum queue size, after which the oldest element is removed
        ///    from the queue. Zero (the default) means unlimited size.
        /// @param[in] overflow_disposition
        ///    How to handle `.put()` invocations into a full queue
        /// @param[in] closewatch
        ///    How frequently to check for application shutdown while blocking for input

        BlockingQueue(
            std::size_t maxsize = 0,
            OverflowDisposition overflow_disposition = OverflowDisposition::DISCARD_OLDEST,
            std::chrono::system_clock::duration closewatch = std::chrono::seconds(2))
            : BlockingQueueBase(maxsize, overflow_disposition),
              closewatch(closewatch)
        {
        }

        void clear() override
        {
            std::unique_lock<std::mutex> lock(this->mtx);
            std::queue<T> other;
            std::swap(this->queue, other);
        }

        // Resolve base for multiply-inherited method `close()`
        void close() override
        {
            BlockingQueueBase::close();
        }

        inline std::size_t size() const override
        {
            return this->queue.size();
        }

        inline bool empty() const override
        {
            return this->queue.empty();
        }

    protected:
        inline void discard_oldest() override
        {
            this->queue.pop();
        }

    public:
        /// @brief
        ///     Add an item to the end of the queue
        /// @param[in] value
        ///     Value to copy to the end of the queue. \sa void put(T && value).
        /// @param[in] reopen
        ///     Reopen the queue if it had been closed
        /// @param[in] notify
        ///     Notify anyone waiting for items.  Disabling this may
        ///     allow for multiple pushes without waiting.
        /// @return
        ///     Whether the item was actually pushed
        ///
        /// Add a new item at the end of the queue.  This unblocks exactly one
        /// pending or future `.get()` call.
        ///
        /// If the queue is full (i.e. if there is a maximum queue size and it
        /// has been reached) then the oldest item is removed from the front of
        /// the queue.

        inline bool put(const T &value,
                        bool reopen = false,
                        bool notify = true)
        {
            if (reopen || !this->closed_)
            {
                {
                    std::unique_lock<std::mutex> lock(this->mtx);
                    this->closed_ = false;
                    if (this->pushable(&lock))
                    {
                        this->queue.push(value);
                    }
                }
                if (notify)
                {
                    this->item_available.notify_one();
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        /// @brief
        ///     Add an item to the queue
        /// @param[in] value
        ///     Value which is moved to the end of the queue.
        /// @param[in] reopen
        ///     Reopen the queue if it had been closed
        /// @param[in] notify
        ///     Notify anyone waiting for items.  Disabling this may
        ///     allow for multiple pushes without waiting.
        /// @return
        ///     Whether the item was actually pushed
        ///
        /// Add a new item at the end of the queue.  This unblocks exactly one
        /// pending or future `.get()` call.
        ///
        /// If the queue is full (i.e. if there is a maximum queue size and it
        /// has been reached) then the oldest item is removed from the front of
        /// the queue.

        inline bool put(T &&value,
                        bool reopen = false,
                        bool notify = true)
        {
            if (reopen || !this->closed_)
            {
                {
                    std::unique_lock<std::mutex> lock(this->mtx);
                    this->closed_ = false;
                    if (this->pushable(&lock))
                    {
                        this->queue.push(std::move(value));
                    }
                }
                if (notify)
                {
                    this->item_available.notify_one();
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        /// @brief
        ///     Notify recipients of available value(s). This may be used after
        ///     `push()` operations where the `notify` flag was set to `false`.
        inline void notify()
        {
            this->item_available.notify_one();
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

        inline std::optional<T> get() override
        {
            std::optional<T> value;
            {
                std::unique_lock<std::mutex> lock(this->mtx);
                while (this->queue.empty() &&
                       !this->closed_ &&
                       !platform::signal_shutdown.emitted())
                {
                    this->item_available.wait_for(lock, this->closewatch);
                }

                if (!this->queue.empty())
                {
                    value = std::move(this->queue.front());
                    this->discard_oldest();
                }
            }
            this->space_available.notify_one();
            return value;
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
            std::optional<T> value;

            {
                std::unique_lock<std::mutex> lock(this->mtx);
                this->item_available.wait_until(lock, deadline, [&] {
                    return this->queue.size() || this->closed_;
                });

                if (this->queue.size())
                {
                    value = std::move(this->queue.front());
                    this->discard_oldest();
                }
            }

            this->space_available.notify_one();
            return value;
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
        std::chrono::system_clock::duration closewatch;
    };

} // namespace core::types
