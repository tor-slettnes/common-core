/// -*- c++ -*-
//==============================================================================
/// @file logger-sink.h++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-base.h++"
#include "logging/sinks/logsink.h++"
#include "thread/blockingqueue.h++"
#include "types/create-shared.h++"

namespace logger
{
    //--------------------------------------------------------------------------
    // QueueSink: Log sink that appends incoming events to its own queue

    class QueueSink
        : public core::logging::LogSink,
          public EventQueue,
          public core::types::enable_create_shared_from_this<QueueSink>
    {
        using This = QueueSink;
        using Super = core::logging::LogSink;

    protected:
        QueueSink(
            const SinkID &sink_id,
            core::status::Level threshold,
            const std::optional<ContractID> &contract_id = {},
            unsigned int maxsize = 0,
            OverflowDisposition overflow_disposition = OverflowDisposition::DISCARD_OLDEST);

    protected:
        void capture_event(const core::status::Event::ptr &event) override;
    };

    //--------------------------------------------------------------------------
    // QueueListener: Listen for log events via QueueSink

    class QueueListener
        : public BaseListener,
          public core::types::enable_create_shared<QueueListener>
    {
        std::shared_ptr<EventQueue> listen(
            const ListenerSpec &spec) override;
    };

}  // namespace logger
