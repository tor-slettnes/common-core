/// -*- c++ -*-
//==============================================================================
/// @file async-dispatcher.h++
/// @brief Distribute a captured item to available sinks - asynchronous flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dispatcher.h++"
#include "thread/blockingqueue.h++"

namespace core::logging
{
    class AsyncDispatcher : public Dispatcher
    {
        using Super = Dispatcher;

    public:
        void initialize() override;
        void deinitialize() override;

        void submit(const types::Loggable::ptr &item) override;

    private:
        void worker();

    private:
        std::thread workerthread_;
        types::BlockingQueue<types::Loggable::ptr> queue_;
    };

}  // namespace core::logging
