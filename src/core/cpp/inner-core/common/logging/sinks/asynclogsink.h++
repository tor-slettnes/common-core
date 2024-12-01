/// -*- c++ -*-
//==============================================================================
/// @file asynclogsink.h++
/// @brief Abstract base for asynchronous log sinks, derived from `LogSink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logsink.h++"
#include "thread/blockingqueue.h++"

namespace core::logging
{
    //==========================================================================
    /// @class AsyncLogSink
    /// @brief Base for logging text

    class AsyncLogSink : public LogSink
    {
        using This = AsyncLogSink;
        using Super = LogSink;

    protected:
        using LogSink::LogSink;

    public:
        void open() override;
        void close() override;
        bool capture(const types::Loggable::ptr &item) override;

    protected:
        virtual void worker();

    private:
        std::thread workerthread_;

    protected:
        types::BlockingQueue<status::Event::ptr> queue;
    };
}  // namespace core::logging
