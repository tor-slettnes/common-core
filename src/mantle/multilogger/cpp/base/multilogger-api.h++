// -*- c++ -*-
//==============================================================================
/// @file multilogger-api.h++
/// @brief Logging service - abstract base
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-types.h++"
#include "logging/message/scope.h++"
#include "thread/signaltemplate.h++"
#include "types/loggable.h++"

#include <optional>

namespace multilogger
{
    define_log_scope("multilogger");

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

        virtual void submit(
            const core::types::Loggable::ptr &item) = 0;

        virtual bool add_sink(
            const SinkSpec &spec) = 0;

        virtual bool remove_sink(
            const SinkID &id) = 0;

        virtual SinkSpec get_sink_spec(
            const SinkID &id) const = 0;

        virtual SinkSpecs get_all_sink_specs() const = 0;

        virtual SinkIDs list_sinks() const = 0;

        virtual SinkTypes list_sink_types() const = 0;

        virtual FieldNames list_message_fields() const = 0;

        virtual FieldNames list_error_fields() const = 0;

        virtual std::shared_ptr<LogSource> listen(
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
        std::weak_ptr<LogSource> listener_;
    };

    extern core::signal::DataSignal<core::types::Loggable::ptr> signal_log_item;

}  // namespace multilogger
