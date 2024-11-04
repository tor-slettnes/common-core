/// -*- c++ -*-
//==============================================================================
/// @file logger-native-listener.c++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-native-listener.h++"
#include "logging/logging.h++"
#include "settings/settings.h++"

namespace logger::native
{
    EventListener::EventListener(
        const SinkID &sink_id,
        core::status::Level threshold,
        const std::optional<core::status::Event::ContractID> &contract_id,
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : LogSink(sink_id, threshold, contract_id),
          BlockingQueue(maxsize, overflow_disposition)
    {
    }

    void EventListener::capture_event(const core::status::Event::ptr &event)
    {
        this->put(event);
    }

}  // namespace logger
