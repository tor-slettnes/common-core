// -*- c++ -*-
//==============================================================================
/// @file logger-api.h++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-types.h++"
#include "status/event.h++"
#include "thread/signaltemplate.h++"
#include "logging/logging.h++"

#include <optional>

namespace logger
{
    define_log_scope("logger");

    //--------------------------------------------------------------------------
    // API

    class API
    {
    protected:
        API(const std::string &identity);
        virtual ~API();

    public:
        const std::string &identity() const;

        virtual void initialize() {}
        virtual void deinitialize() {}

        virtual void log(
            const core::status::Event::ptr &event) = 0;

        virtual bool add_sink(
            const SinkSpec &spec) = 0;

        virtual bool remove_sink(
            const SinkID &id) = 0;

        virtual SinkSpec get_sink_spec(
            const SinkID &id) const = 0;

        virtual SinkSpecs list_sinks() const = 0;

        virtual FieldNames list_static_fields() const = 0;

        virtual std::shared_ptr<EventSource> listen(
            const ListenerSpec &spec) = 0;

    public:
        void start_listening(const ListenerSpec &spec);
        void stop_listening();

    private:
        void keep_listening(const ListenerSpec &spec);

    private:
        std::string identity_;
        bool keep_listening_;
        std::thread listener_thread_;
        std::weak_ptr<EventSource> listener_;
    };


    extern core::signal::DataSignal<core::status::Event::ptr> signal_log_event;

}  // namespace logger
