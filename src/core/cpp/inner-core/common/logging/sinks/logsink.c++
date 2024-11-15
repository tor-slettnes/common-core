/// -*- c++ -*-
//==============================================================================
/// @file logsink.c++
/// @brief Abstract base for log sinks, derived from `capture::Sink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logsink.h++"

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
        if (auto threshold = settings.get_as<status::Level>(SETTING_THRESHOLD))
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
        if (auto event = std::dynamic_pointer_cast<status::Event>(item))
        {
            this->capture_event(event);
            return true;
        }
        return false;
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
