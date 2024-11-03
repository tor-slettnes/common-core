/// -*- c++ -*-
//==============================================================================
/// @file logsink.h++
/// @brief Abstract base for log sinks, derived from `capture::Sink()`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sink.h++"
#include "status/event.h++"

#include <string>

namespace core::logging
{
    //==========================================================================
    /// \class LogSink
    /// \brief Base for logging text

    class LogSink : public Sink
    {
        using This = LogSink;
        using Super = Sink;

    public:
        using ptr = std::shared_ptr<LogSink>;

    protected:
        LogSink(const SinkID &sink_id,
                status::Level threshold,
                const std::optional<status::Event::ContractID> contract_id = {});

    public:
        bool is_applicable(const types::Loggable &item) const override;
        bool capture(const types::Loggable::ptr &item) override;

        virtual void set_threshold(status::Level threshold);
        status::Level threshold() const;

        virtual void set_contract_id(std::optional<status::Event::ContractID> contract_id);
        std::optional<status::Event::ContractID> contract_id() const;

    protected:
        virtual void capture_event(const status::Event::ptr &event) = 0;

    private:
        status::Level threshold_;
        std::optional<status::Event::ContractID> contract_id_;
    };
}  // namespace core::logging
