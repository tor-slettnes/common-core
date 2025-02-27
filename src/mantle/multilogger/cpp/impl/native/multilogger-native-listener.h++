/// -*- c++ -*-
//==============================================================================
/// @file multilogger-native-listener.h++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "logging/sinks/sink.h++"
#include "thread/blockingqueue.h++"
#include "types/create-shared.h++"

namespace multilogger::native
{
    class QueueListener
        : public core::logging::Sink,
          public core::types::BlockingQueue<core::types::Loggable::ptr>,
          public core::types::enable_create_shared_from_this<QueueListener>
    {
        using This = QueueListener;
        using Super = core::logging::Sink;

    protected:
        QueueListener(
            const SinkID &sink_id,
            core::status::Level threshold,
            const std::optional<Loggable::ContractID> &contract_id = {},
            unsigned int maxsize = 0,
            OverflowDisposition overflow_disposition = OverflowDisposition::DISCARD_OLDEST);

        ~QueueListener();

    public:
        void open() override;
        void close() override;

    protected:
        bool handle_item(const core::types::Loggable::ptr &item) override;
    };
}  // namespace multilogger::native
