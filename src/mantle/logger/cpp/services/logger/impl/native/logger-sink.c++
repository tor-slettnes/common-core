/// -*- c++ -*-
//==============================================================================
/// @file logger-sink.c++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-sink.h++"
#include "logging/logging.h++"
#include "settings/settings.h++"

namespace logger
{
    //--------------------------------------------------------------------------
    // QueueSink: Log sink that appends incoming events to its own queue

    QueueSink::QueueSink(
        const SinkID &sink_id,
        core::status::Level threshold,
        const std::optional<core::status::Event::ContractID> &contract_id,
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : LogSink(sink_id, threshold, contract_id),
          BlockingQueue(maxsize, overflow_disposition)
    {
    }

    void QueueSink::capture_event(const core::status::Event::ptr &event)
    {
        this->put(event);
    }

    //--------------------------------------------------------------------------
    // QueueListener: Listen for log events via QueueSink

    std::shared_ptr<EventQueue> QueueListener::listen(
        const ListenerSpec &spec)
    {
        auto sink = QueueSink::create_shared(
            spec.sink_id,
            spec.min_level,
            spec.contract_id,
            core::settings->get("log sinks").get("client").get("queue size", 4096).as_uint());
        sink->open();
        return sink;
    }

}  // namespace logger
