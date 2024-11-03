// -*- c++ -*-
//==============================================================================
/// @file logger-native.c++
/// @brief Logging service - host native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-native.h++"
#include "logger-sink.h++"
#include "logging/logging.h++"
#include "logging/sinks/logfilesink.h++"
#include "logging/sinks/jsonfilesink.h++"
#include "logging/sinks/csvfilesink.h++"
#include "platform/logsink.h++"
#include "status/exceptions.h++"
#include "settings/settings.h++"

namespace logger
{
    NativeLogger::NativeLogger(const std::string &identity)
        : Super(identity)
    {
    }

    void NativeLogger::log(
        const core::status::Event::ptr &event)
    {
        core::logging::message_dispatcher.submit(event);
    }

    bool NativeLogger::add_sink(
        const SinkSpec &spec)
    {
        core::logging::Sink::ptr sink = core::logging::message_dispatcher.get_sink(spec.sink_id);

        if (!sink)
        {
            sink = this->new_sink(spec);
            sink->open();
            return core::logging::message_dispatcher.add_sink(sink) != nullptr;
        }
        else
        {
            return false;
        }
    }

    bool NativeLogger::remove_sink(
        const SinkID &id)
    {
        return core::logging::message_dispatcher.remove_sink(id);
    }

    SinkSpec NativeLogger::get_sink_spec(
        const SinkID &id) const
    {
        if (core::logging::Sink::ptr sink = core::logging::message_dispatcher.get_sink(id))
        {
            return this->sink_spec(sink);
        }
        else
        {
            return {};
        }
    }

    SinkSpecs NativeLogger::list_sinks() const
    {
        return {};
    }

    FieldNames NativeLogger::list_static_fields() const
    {
        return core::logging::Message::field_names();
    }

    core::logging::Sink::ptr NativeLogger::new_sink(const SinkSpec &spec) const
    {
        switch (spec.sink_type)
        {
        case SinkType::LOGFILE:
            return core::logging::LogFileSink::create_shared(
                spec.sink_id,            // sink_id
                spec.min_level,          // threshold
                spec.filename_template,  // path_template
                spec.rotation_interval,  // rotation_interval
                spec.use_local_time);    // local_time

        case SinkType::JSON:
            return core::logging::JsonFileSink::create_shared(
                spec.sink_id,            // sink_id
                spec.min_level,          // threshold
                spec.filename_template,  // path_template
                spec.rotation_interval,  // rotation_interval
                spec.use_local_time);    // local_time

        case SinkType::CSV:
            return core::logging::CSVEventSink::create_shared(
                spec.sink_id,            // sink_id
                spec.min_level,          // threshold
                spec.contract_id,        // contract_id
                spec.fields,             // columns
                spec.filename_template,  // path_template
                spec.rotation_interval,  // rotation_interval
                spec.use_local_time);    // local_time

        case SinkType::DB:
            return {};

        default:
            throw core::exception::InvalidArgument(
                "Unsupported sink type",
                static_cast<int>(spec.sink_type));
        }
    }

    SinkSpec NativeLogger::sink_spec(const core::logging::Sink::ptr &sink) const
    {
        if (auto syslogsink = std::dynamic_pointer_cast<core::platform::LogSinkProvider>(sink))
        {
            return {
                .sink_id = syslogsink->sink_id(),
                .sink_type = SinkType::SYSLOG,
                .min_level = syslogsink->threshold(),
                .contract_id = syslogsink->contract_id(),
            };
        }

        else if (auto filesink = std::dynamic_pointer_cast<core::logging::LogFileSink>(sink))
        {
            return {
                .sink_id = filesink->sink_id(),
                .sink_type = SinkType::LOGFILE,
                .filename_template = filesink->path_template(),
                .rotation_interval = filesink->rotation_interval(),
                .use_local_time = filesink->use_local_time(),
                .min_level = filesink->threshold(),
                .contract_id = filesink->contract_id(),
            };
        }

        else if (auto jsonsink = std::dynamic_pointer_cast<core::logging::JsonFileSink>(sink))
        {
            return {
                .sink_id = jsonsink->sink_id(),
                .sink_type = SinkType::JSON,
                .filename_template = jsonsink->path_template(),
                .rotation_interval = jsonsink->rotation_interval(),
                .use_local_time = jsonsink->use_local_time(),
                .min_level = jsonsink->threshold(),
                .contract_id = jsonsink->contract_id(),
            };
        }

        else if (auto csvsink = std::dynamic_pointer_cast<core::logging::CSVBaseSink>(sink))
        {
            return {
                .sink_id = csvsink->sink_id(),
                .sink_type = SinkType::CSV,
                .filename_template = csvsink->path_template(),
                .rotation_interval = csvsink->rotation_interval(),
                .use_local_time = csvsink->use_local_time(),
                .min_level = csvsink->threshold(),
                .contract_id = csvsink->contract_id(),
                .fields = csvsink->column_defaults(),
            };
        }
        return {};
    }
}  // namespace logger
