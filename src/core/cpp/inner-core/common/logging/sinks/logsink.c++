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
                     status::Level threshold,
                     const std::optional<status::Event::ContractID> contract_id)
        : Sink(sink_id),
          threshold_(threshold),
          contract_id_(contract_id)
    {
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
