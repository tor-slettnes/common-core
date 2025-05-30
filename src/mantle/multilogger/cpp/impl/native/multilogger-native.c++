// -*- c++ -*-
//==============================================================================
/// @file multilogger-native.c++
/// @brief Logging service - host native implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-native.h++"
#include "multilogger-native-listener.h++"
#if USE_SQLITE3
#include "multilogger-sqlite3-sink.h++"
#endif
#include "logging/logging.h++"
#include "logging/sinks/core-sinks.h++"
#include "platform/logsink.h++"
#include "status/exceptions.h++"
#include "status/error.h++"
#include "settings/settings.h++"

namespace multilogger::native
{
    void Logger::submit(
        const core::types::Loggable::ptr &item)
    {
        core::logging::dispatcher.submit(item);
    }

    bool Logger::add_sink(
        const SinkSpec &spec)
    {
        core::logging::Sink::ptr sink = core::logging::dispatcher.get_sink(spec.sink_id);

        if (!sink)
        {
            sink = this->new_sink(spec);
            sink->open();
            return (core::logging::dispatcher.add_sink(sink) != nullptr);
        }
        return false;
    }

    bool Logger::remove_sink(
        const SinkID &id)
    {
        if (core::logging::sink_registry.get(id))
        {
            throw core::exception::InvalidArgument(
                "Requested Sink ID is reserved for default log sink of "
                "the corresponding type and cannot be removed.",
                id);
        }

        return core::logging::dispatcher.remove_sink(id);
    }

    SinkSpec Logger::get_sink_spec(
        const SinkID &id) const
    {
        if (core::logging::Sink::ptr sink = core::logging::dispatcher.get_sink(id))
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
        specs.reserve(core::logging::dispatcher.sinks().size());

        for (const auto &[sink_id, sink] : core::logging::dispatcher.sinks())
        {
            specs.push_back(this->sink_spec(sink));
        }

        return specs;
    }

    SinkIDs Logger::list_sinks() const
    {
        return core::logging::dispatcher.sinks().keys();
    }

    SinkTypes Logger::list_sink_types() const
    {
        return core::logging::sink_registry.keys();
    }

    FieldNames Logger::list_message_fields() const
    {
        return core::logging::Message::message_fields();
    }

    FieldNames Logger::list_error_fields() const
    {
        return core::status::Error::error_fields();
    }

    std::shared_ptr<LogSource> Logger::listen(
        const ListenerSpec &spec)
    {
        std::size_t queue_size = core::settings
                                     ->get("log sinks")
                                     .get("multilogger")
                                     .get("queue size", 4096)
                                     .as_uint();

        auto sink = QueueListener::create_shared(
            spec.sink_id,
            spec.min_level,
            spec.contract_id,
            spec.hosts,
            spec.applications,
            queue_size);

        sink->open();
        return sink;
    }

    core::logging::Sink::ptr Logger::new_sink(const SinkSpec &spec) const
    {
        if (core::logging::sink_registry.get(spec.sink_id))
        {
            throw core::exception::InvalidArgument(
                "Requested Sink ID is reserved for default log sink of the corresponding type.",
                spec.sink_id);
        }
        else if (auto *factory = core::logging::sink_factories.get(spec.sink_type))
        {
            return this->create_sink(factory, spec);
        }
        else
        {
            throw core::exception::OutOfRange(
                "Unsupported sink type",
                spec.sink_type);
        }
    }

    core::logging::Sink::ptr Logger::create_sink(core::logging::SinkFactory *factory,
                                                 const SinkSpec &spec) const
    {
        core::logging::Sink::ptr sink = factory->create_sink(spec.sink_id);

        sink->set_threshold(spec.min_level);
        sink->set_contract_id(spec.contract_id);

        if (auto rotating_path = std::dynamic_pointer_cast<core::logging::RotatingPath>(sink))
        {
            rotating_path->set_filename_template(spec.filename_template);
            rotating_path->set_rotation_interval(spec.rotation_interval);
            rotating_path->set_use_local_time(spec.use_local_time);
            rotating_path->set_compress_after_use(spec.compress_after_use);
            rotating_path->set_expiration_interval(spec.expiration_interval);
        }

        if (auto tabular_data = std::dynamic_pointer_cast<core::logging::TabularData>(sink))
        {
            tabular_data->set_columns(spec.columns);
        }
        return sink;
    }

    SinkSpec Logger::sink_spec(const core::logging::Sink::ptr &sink) const
    {
        SinkSpec spec = {
            .sink_id = sink->sink_id(),
            .sink_type = sink->sink_type(),
            .contract_id = sink->contract_id(),
            .min_level = sink->threshold(),
        };

        if (auto rpath = std::dynamic_pointer_cast<core::logging::RotatingPath>(sink))
        {
            spec.filename_template = rpath->filename_template();
            spec.rotation_interval = rpath->rotation_interval();
            spec.use_local_time = rpath->use_local_time();
            spec.compress_after_use = rpath->compress_after_use();
            spec.expiration_interval = rpath->expiration_interval();
        }

        if (auto tabdata = std::dynamic_pointer_cast<core::logging::TabularData>(sink))
        {
            spec.columns = tabdata->columns();
        }

        return spec;
    }
}  // namespace multilogger::native
