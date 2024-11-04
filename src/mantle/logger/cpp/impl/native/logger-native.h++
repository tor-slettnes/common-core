// -*- c++ -*-
//==============================================================================
/// @file logger-native.h++
/// @brief Logging service - host native interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-api.h++"
#include "types/create-shared.h++"

namespace logger::native
{
    class Logger : public API,
                   public core::types::enable_create_shared<Logger>
    {
        using This = Logger;
        using Super = API;

    protected:
        Logger(const std::string &identity);

    public:
        void log(const core::status::Event::ptr &event) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs list_sinks() const override;
        FieldNames list_static_fields() const override;

        std::shared_ptr<EventSource> listen(
            const ListenerSpec &spec) override;

    protected:
        core::logging::Sink::ptr new_sink(const SinkSpec &spec) const;
        SinkSpec sink_spec(const core::logging::Sink::ptr &sink) const;
    };
}  // namespace logger::native
