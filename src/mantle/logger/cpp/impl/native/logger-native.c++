// -*- c++ -*-
//==============================================================================
/// @file logger-native.c++
/// @brief Logging service - host native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-native.h++"
#include "logger-native-listener.h++"
#include "logger-sqlite3-sink.h++"
#include "logging/logging.h++"
#include "logging/sinks/streamsink.h++"
#include "logging/sinks/logfilesink.h++"
#include "logging/sinks/jsonfilesink.h++"
#include "logging/sinks/csvfilesink.h++"
#include "platform/logsink.h++"
#include "status/exceptions.h++"
#include "settings/settings.h++"

namespace logger::native
{
    Logger::Logger(const std::string &identity)
        : Super(identity)
    {
    }

    void Logger::submit(
        const core::status::Event::ptr &event)
    {
        core::logging::message_dispatcher.submit(event);
    }

    bool Logger::add_sink(
        const SinkSpec &spec)
    {
        core::logging::Sink::ptr sink = core::logging::message_dispatcher.get_sink(spec.sink_id);

        if (!sink)
        {
            if (sink = this->new_sink(spec))
            {
                sink->open();
                return core::logging::message_dispatcher.add_sink(sink) != nullptr;
            }
        }
        return false;
    }

    bool Logger::remove_sink(
        const SinkID &id)
    {
        return core::logging::message_dispatcher.remove_sink(id);
    }

    SinkSpec Logger::get_sink_spec(
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

    SinkSpecs Logger::get_all_sink_specs() const
    {
        SinkSpecs specs;
        specs.reserve(core::logging::message_dispatcher.sinks().size());

        for (const auto &[sink_id, sink] : core::logging::message_dispatcher.sinks())
        {
            specs.push_back(this->sink_spec(sink));
        }

        return specs;
    }

    SinkIDs Logger::list_sinks() const
    {
        SinkIDs sink_ids;
        sink_ids.reserve(core::logging::message_dispatcher.sinks().size());
        for (const auto &[sink_id, sink]: core::logging::message_dispatcher.sinks())
        {
            sink_ids.push_back(sink_id);
        }

        return sink_ids;
    }

    FieldNames Logger::list_static_fields() const
    {
        return core::logging::Message::field_names();
    }

    std::shared_ptr<EventSource> Logger::listen(
        const ListenerSpec &spec)
    {
        auto sink = EventListener::create_shared(
            spec.sink_id,
            spec.min_level,
            spec.contract_id,
            core::settings->get("log sinks").get("client").get("queue size", 4096).as_uint());

        sink->open();
        return sink;
    }

    core::logging::Sink::ptr Logger::new_sink(const SinkSpec &spec) const
    {
        core::logging::Sink::ptr sink;

        switch (spec.sink_type)
        {
        case SinkType::LOGFILE:
            sink = core::logging::LogFileSink::create_shared(spec.sink_id);
            break;

        case SinkType::JSON:
            sink = core::logging::JsonFileSink::create_shared(spec.sink_id);
            break;

        case SinkType::CSV:
            sink = core::logging::CSVEventSink::create_shared(spec.sink_id);
            break;

        case SinkType::DB:
            break;

        default:
            throw core::exception::InvalidArgument(
                "Unsupported sink type",
                static_cast<int>(spec.sink_type));
        }

        if (auto logsink = std::dynamic_pointer_cast<core::logging::LogSink>(sink))
        {
            logsink->set_threshold(spec.min_level);
            logsink->set_contract_id(spec.contract_id);
        }

        if (auto rotating_path = std::dynamic_pointer_cast<core::logging::RotatingPath>(sink))
        {
            rotating_path->set_filename_template(spec.filename_template);
            rotating_path->set_rotation_interval(spec.rotation_interval);
            rotating_path->set_use_local_time(spec.use_local_time);
        }

        if (auto tabular_data = std::dynamic_pointer_cast<core::logging::TabularData>(sink))
        {
            tabular_data->set_columns(spec.columns);
        }

        // if (auto db_sink = std::dynamic_pointer_cast<logger::SQLiteSink>(sink))
        // {
        // }

        return sink;
    }

    SinkSpec Logger::sink_spec(const core::logging::Sink::ptr &sink) const
    {
        if (auto streamsink = std::dynamic_pointer_cast<core::logging::StreamSink>(sink))
        {
            return {
                .sink_id = streamsink->sink_id(),
                .sink_type = SinkType::STREAM,
                .min_level = streamsink->threshold(),
                .contract_id = streamsink->contract_id(),
            };
        }

        else if (auto syslogsink = std::dynamic_pointer_cast<core::platform::LogSinkProvider>(sink))
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
                .filename_template = filesink->filename_template(),
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
                .filename_template = jsonsink->filename_template(),
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
                .filename_template = csvsink->filename_template(),
                .rotation_interval = csvsink->rotation_interval(),
                .use_local_time = csvsink->use_local_time(),
                .min_level = csvsink->threshold(),
                .contract_id = csvsink->contract_id(),
                .columns = csvsink->columns(),
            };
        }
        else
        {
            return {
                .sink_id = sink->sink_id(),
            };
        }
    }
}  // namespace logger::native
