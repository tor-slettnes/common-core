// -*- c++ -*-
//==============================================================================
/// @file logger-types.c++
/// @brief Logging service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-types.h++"
#include "types/symbolmap.h++"

namespace logger
{

    // Sink types that can be added by remote client
    core::types::SymbolMap<SinkType> sinktype_names = {
        {SinkType::SYSLOG, "syslog"},
        {SinkType::LOGFILE, "file"},
        {SinkType::JSON, "json"},
        {SinkType::CSV, "csv"},
        {SinkType::DB, "db"},
    };

    std::ostream &operator<<(std::ostream &stream, SinkType sink_type)
    {
        return sinktype_names.to_stream(stream, sink_type);
    }

    std::istream &operator>>(std::istream &stream, SinkType &sink_type)
    {
        return sinktype_names.from_stream(stream, &sink_type);
    }

    std::ostream &operator<<(std::ostream &stream, const SinkSpec &spec)
    {
        core::types::PartsList parts;
        parts.add_string("sink_id", spec.sink_id);
        parts.add("sink_type", spec.sink_type);
        parts.add_value("persistent", spec.persistent, spec.persistent);
        parts.add_string("filename_template", spec.filename_template);
        parts.add("rotation_interval", spec.rotation_interval, spec.rotation_interval);
        parts.add_value("use_local_time", spec.use_local_time);
        parts.add("min_level", spec.min_level, spec.min_level != core::status::Level::NONE);
        parts.add_string("contract_id", spec.contract_id.value_or(""));
        parts.add_value("fields", spec.fields, !spec.fields.empty());

        stream << parts;
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const ListenerSpec &spec)
    {
        core::types::PartsList parts;
        parts.add_string("sink_id", spec.sink_id);
        parts.add("min_level", spec.min_level, spec.min_level != core::status::Level::NONE);
        parts.add_string("contract_id", spec.contract_id.value_or(""));
        stream << parts;
        return stream;
    }

}  // namespace logger
