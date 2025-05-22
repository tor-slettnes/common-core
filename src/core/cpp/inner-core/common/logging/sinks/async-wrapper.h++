/// -*- c++ -*-
//==============================================================================
/// @file async-sink.h++
/// @brief Capture/process loggable items in an asynchronous queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/loggable.h++"
#include "thread/blockingqueue.h++"

#include <memory>
#include <thread>
#include <optional>

namespace core::logging
{
    constexpr const auto SETTING_QUEUE_SIZE = "queue_size";
    constexpr const auto DEFAULT_QUEUE_SIZE = 0;

    //--------------------------------------------------------------------------
    // @class AsyncWrapper
    // @brief Sink wrapper to enqueue loggable items and handle in a separate thread

    template <class SinkType>
    class AsyncWrapper : public SinkType
    {

        using This = AsyncWrapper<SinkType>;
        using Super = SinkType;

        using LoggableQueue = types::BlockingQueue<types::Loggable::ptr>;

    protected:
        using Super::Super;
        virtual ~AsyncWrapper()
        {
            this->close_async_queue();
            if (this->workerthread_.joinable())
            {
                str::format(std::cout,
                            "Sink %r AsynWrapper destructor waiting for worker thread\n",
                            this->sink_id());

                this->workerthread_.join();
            }
        }

    protected:
        void load_settings(const types::KeyValueMap &settings) override
        {
            Super::load_settings(settings);
            this->load_async_settings(settings);
        }

        void load_async_settings(const types::KeyValueMap &settings)
        {
            if (const types::Value &value = settings.get(SETTING_QUEUE_SIZE))
            {
                this->set_queue_size(value.as_uint64());
            }
        }

    public:
        std::size_t queue_size() const
        {
            return this->queue_size_;
        }

        void set_queue_size(std::size_t size)
        {
            this->queue_size_ = size;
        }

    protected:
        bool is_open() const override
        {
            return this->queue_ && !this->queue_->closed();
        }

        void open() override
        {
            this->open_async_queue();
            Super::open();
        }

        void close() override
        {
            Super::close();
            this->close_async_queue();
        }

        bool capture(const types::Loggable::ptr &loggable) override
        {
            if (this->is_open())
            {
                this->queue_->put(loggable);
                return true;
            }
            else
            {
                return false;
            }
        }

        void open_async_queue()
        {
            if (!this->is_open())
            {
                this->queue_ = std::make_unique<LoggableQueue>(this->queue_size());
                this->workerthread_ = std::thread(&This::worker, this);
            }
        }

        void close_async_queue()
        {
            if (this->is_open())
            {
                this->queue_->close();
            }
        }

    public:
        virtual void worker()
        {
            while (const std::optional<types::Loggable::ptr> &opt_item = this->queue_->get())
            {
                this->try_handle_item(opt_item.value());
            }
        }

    protected:
        std::shared_ptr<LoggableQueue> queue() const
        {
            return this->queue_;
        }

    private:
        std::thread workerthread_;
        std::shared_ptr<LoggableQueue> queue_;
        std::size_t queue_size_ = 0;
    };
}  // namespace core::logging
