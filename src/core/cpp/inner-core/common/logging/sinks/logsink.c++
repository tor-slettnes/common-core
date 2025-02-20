/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief Abstract base for log sinks, derived from `capture::Sink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logsink.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::logging
{
    //==========================================================================
    // LogSink

    LogSink::LogSink(const std::string &sink_id,
                     const SinkType &sink_type)
        : Sink(sink_id, sink_type),
          threshold_(status::Level::NONE)
    {
    }

    void LogSink::load_settings(const types::KeyValueMap &settings)
    {
        Super::load_settings(settings);

        if (auto threshold = settings.try_get_as<status::Level>(SETTING_THRESHOLD))
        {
            this->set_threshold(threshold.value());
        }

        if (const core::types::Value &contract_id = settings.get(SETTING_CONTRACT_ID))
        {
            this->set_contract_id(contract_id.as_string());
        }
    }

    bool LogSink::is_applicable(const types::Loggable &item) const
    {
        if (auto event = dynamic_cast<const status::Event *>(&item))
        {
            return (event->level() >= this->threshold()) &&
                   (!this->contract_id().has_value() ||
                    (event->contract_id() == this->contract_id().value()));
        }
        else
        {
            return false;
        }
    }

    bool LogSink::capture(const types::Loggable::ptr &item)
    {
        if (this->is_open())
        {
            if (auto event = std::dynamic_pointer_cast<status::Event>(item))
            {
                return this->try_capture_event(event);
            }
        }
        return false;
    }

    bool LogSink::try_capture_event(const status::Event::ptr &event)
    {
        try
        {
            this->capture_event(event);
            return true;
        }
        catch (...)
        {
            this->close();
            logf_error("Log sink %r failed to capture event: %s",
                       this->sink_id(),
                       std::current_exception());
            return false;
        }
    }

    void LogSink::set_threshold(status::Level threshold)
    {
        this->threshold_ = threshold;
    }

    status::Level LogSink::threshold() const
    {
        return this->threshold_;
    }

    void LogSink::set_contract_id(std::optional<status::Event::ContractID> contract_id)
    {
        this->contract_id_ = contract_id;
    }

    std::optional<status::Event::ContractID> LogSink::contract_id() const
    {
        return this->contract_id_;
    }

}  // namespace core::logging
