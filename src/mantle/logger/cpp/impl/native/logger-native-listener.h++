/// -*- c++ -*-
//==============================================================================
/// @file logger-native-listener.h++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-api.h++"
#include "logging/sinks/logsink.h++"
#include "thread/blockingqueue.h++"
#include "types/create-shared.h++"

namespace logger::native
{
    class EventListener
        : public core::logging::LogSink,
          public core::types::BlockingQueue<core::status::Event::ptr>,
          public core::types::enable_create_shared_from_this<EventListener>
    {
        using This = EventListener;
        using Super = core::logging::LogSink;

    protected:
        EventListener(
            const SinkID &sink_id,
            core::status::Level threshold,
            const std::optional<ContractID> &contract_id = {},
            unsigned int maxsize = 0,
            OverflowDisposition overflow_disposition = OverflowDisposition::DISCARD_OLDEST);

        ~EventListener();

    public:
        void open() override;
        void close() override;

    protected:
        void capture_event(const core::status::Event::ptr &event) override;
    };
}  // namespace logger::native
