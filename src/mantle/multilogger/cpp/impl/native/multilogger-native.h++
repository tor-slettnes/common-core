// -*- c++ -*-
//==============================================================================
/// @file multilogger-native.h++
/// @brief Logging service - host native interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "logging/sinks/factory.h++"
#include "types/create-shared.h++"

namespace multilogger::native
{
    class Logger : public API,
                   public core::types::enable_create_shared<Logger>
    {
        using This = Logger;
        using Super = API;

    protected:
        Logger(const std::string &identity);

    public:
        void submit(const core::status::Event::ptr &event) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs get_all_sink_specs() const override;
        SinkIDs list_sinks() const override;
        SinkTypes list_sink_types() const override;
        FieldNames list_static_fields() const override;

        std::shared_ptr<EventSource> listen(
            const ListenerSpec &spec) override;

    protected:
        core::logging::Sink::ptr new_sink(const SinkSpec &spec) const;
        core::logging::Sink::ptr create_sink(core::logging::SinkFactory *factory,
                                             const SinkSpec &spec) const;

        SinkSpec sink_spec(const core::logging::Sink::ptr &sink) const;

    private:
        std::thread listener_thread_;
        std::shared_ptr<EventSource> listener_;
    };
}  // namespace multilogger::native
