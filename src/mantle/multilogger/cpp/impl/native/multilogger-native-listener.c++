/// -*- c++ -*-
//==============================================================================
/// @file multilogger-native-listener.c++
/// @brief A log sink appending to a queue
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-native-listener.h++"
#include "logging/logging.h++"
#include "settings/settings.h++"

namespace multilogger::native
{
    EventListener::EventListener(
        const SinkID &sink_id,
        core::status::Level threshold,
        const std::optional<core::status::Event::ContractID> &contract_id,
        unsigned int maxsize,
        OverflowDisposition overflow_disposition)
        : LogSink(sink_id),
          BlockingQueue(maxsize, overflow_disposition)
    {
        logf_debug("Created native::EventListener(sink_id=%r, threshold=%s)",
                   sink_id, threshold);

        this->set_threshold(threshold);
        this->set_contract_id(contract_id);

        core::platform::signal_shutdown.connect(
            this->sink_id(),
            std::bind(&EventListener::close, this));
    }

    EventListener::~EventListener()
    {
        core::platform::signal_shutdown.disconnect(this->sink_id());
    }

    void EventListener::open()
    {
        LogSink::open();
        core::logging::dispatcher.add_sink(this->shared_from_this());
    }

    void EventListener::close()
    {
        core::logging::dispatcher.remove_sink(this->shared_from_this());
        BlockingQueue::close();
        LogSink::close();
    }

    void EventListener::capture_event(const core::status::Event::ptr &event)
    {
        this->put(event);
    }

}  // namespace multilogger
