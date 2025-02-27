/// -*- c++ -*-
//==============================================================================
/// @file capture.h++
/// @brief Capture loggable items, synchronously or asynchronously
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/loggable.h++"
#include "types/create-shared.h++"
#include "thread/blockingqueue.h++"

#include <functional>
#include <thread>

namespace core::logging
{
    //--------------------------------------------------------------------------
    // @class Capture
    // @brief Abstract base to handle loggable items

    class Capture
    {
    public:
        using ptr = std::shared_ptr<Capture>;
        using Handler = std::function<bool(const types::Loggable::ptr &)>;

    protected:
        Capture(const Handler handler);

    public:
        virtual void start() {}
        virtual void stop() {}
        virtual bool operator()(const types::Loggable::ptr &item) = 0;

    protected:
        Handler handler;
    };

    //--------------------------------------------------------------------------
    // @class PassthroughCapture
    // @brief Pass on loggable items directly to sink

    class PassthroughCapture : public Capture,
                               public types::enable_create_shared<PassthroughCapture>
    {
        using This = PassthroughCapture;

    public:
        using Capture::Capture;

    protected:
        bool operator()(const types::Loggable::ptr &item) override;
    };

    //--------------------------------------------------------------------------
    // @class AsyncCapture
    // @brief Queue loggable items and pass onto sink in a separate thread

    class AsyncCapture : public Capture,
                         public types::enable_create_shared<PassthroughCapture>
    {
        using This = AsyncCapture;

    public:
        using Capture::Capture;

    protected:
        void start() override;
        void stop() override;
        bool operator()(const types::Loggable::ptr &item) override;
        void worker();

    private:
        std::thread workerthread_;
        types::BlockingQueue<types::Loggable::ptr> queue;

    };


}  // namespace core::logging
