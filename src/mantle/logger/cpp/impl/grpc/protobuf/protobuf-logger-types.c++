// -*- c++ -*-
//==============================================================================
/// @file protobuf-logger-types.h++
/// @brief conversions to/from Protocol Buffer messages for Logger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-datetime-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // SinkType

    void encode(const logger::SinkType &native, cc::logger::SinkType *proto)
    {
        *proto = static_cast<cc::logger::SinkType>(native);
    }

    void decode(const cc::logger::SinkType &proto, logger::SinkType *native)
    {
        *native = static_cast<logger::SinkType>(proto);
    }

    //==========================================================================
    // SinkID

    void encode(const logger::SinkID &native, cc::logger::SinkID *proto)
    {
        proto->set_sink_id(native);
    }

    void decode(const cc::logger::SinkID &proto, logger::SinkID *native)
    {
        *native = proto.sink_id();
    }

    //==========================================================================
    // SinkSpec

    void encode(const logger::SinkSpec &native, cc::logger::SinkSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_sink_type(encoded<cc::logger::SinkType>(native.sink_type));
        proto->set_permanent(native.permanent);
        proto->set_filename_template(native.filename_template);
        encode(native.rotation_interval, proto->mutable_rotation_interval());
        proto->set_use_local_time(native.use_local_time);
        proto->set_min_level(encoded<cc::status::Level>(native.min_level));

        if (native.contract_id.has_value())
        {
            proto->set_contract_id(native.contract_id.value());
        }

        if (!native.columns.empty())
        {
            encode_vector(native.columns, proto->mutable_columns());
        }
    }

    void decode(const cc::logger::SinkSpec &proto, logger::SinkSpec *native)
    {
        native->sink_id = proto.sink_id();
        decode(proto.sink_type(), &native->sink_type);
        native->permanent = proto.permanent();
        native->filename_template = proto.filename_template();
        decode(proto.rotation_interval(), &native->rotation_interval);
        native->use_local_time = proto.use_local_time();
        decode(proto.min_level(), &native->min_level);

        if (proto.has_contract_id())
        {
            native->contract_id = proto.contract_id();
        }

        decode_to_vector(proto.columns(), &native->columns);
    }

    //==========================================================================
    // logger::SinkSpecs <-> cc::logger::SinkSpecs

    void encode(const logger::SinkSpecs &native, cc::logger::SinkSpecs *proto)
    {
        encode_vector(native, proto->mutable_specs());
    }

    void decode(const cc::logger::SinkSpecs &proto, logger::SinkSpecs *native)
    {
        decode_to_vector(proto.specs(), native);
    }

    //==========================================================================
    // logger::SinkIDs <-> cc::logger::SinkSpecs

    void encode(const logger::SinkIDs &native, cc::logger::SinkSpecs *proto)
    {
        auto specs = proto->mutable_specs();
        specs->Reserve(native.size());
        for (const logger::SinkID &sink_id : native)
        {
            cc::logger::SinkSpec *spec = specs->Add();
            spec->set_sink_id(sink_id);
        }
    }

    void decode(const cc::logger::SinkSpecs &proto, logger::SinkIDs *native)
    {
        native->reserve(proto.specs_size());
        for (const cc::logger::SinkSpec &spec : proto.specs())
        {
            native->push_back(spec.sink_id());
        }
    }

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::logger::ColumnSpec *proto)
    {
        proto->set_event_field(native.event_field);
        proto->set_column_name(native.column_name);
        proto->set_column_type(encoded<cc::logger::ColumnType>(native.column_type));
        proto->set_format_string(native.format_string);
    }

    void decode(const cc::logger::ColumnSpec &proto, core::logging::ColumnSpec *native)
    {
        native->event_field = proto.event_field();
        native->column_name = proto.column_name();
        decode(proto.column_type(), &native->column_type);
        native->format_string = proto.format_string();
    }

    //==========================================================================
    // ColumnType

    void encode(const core::logging::ColumnType &native, cc::logger::ColumnType *proto)
    {
        *proto = static_cast<cc::logger::ColumnType>(native);
    }

    void decode(const cc::logger::ColumnType &proto, core::logging::ColumnType *native)
    {
        *native = static_cast<core::logging::ColumnType>(proto);
    }

    //==========================================================================
    // ListenerSpec

    void encode(const logger::ListenerSpec &native, cc::logger::ListenerSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_min_level(encoded<cc::status::Level>(native.min_level));

        if (native.contract_id.has_value())
        {
            proto->set_contract_id(native.contract_id.value());
        }
    }

    void decode(const cc::logger::ListenerSpec &proto, logger::ListenerSpec *native)
    {
        native->sink_id = proto.sink_id();
        decode(proto.min_level(), &native->min_level);

        if (proto.has_contract_id())
        {
            native->contract_id = proto.contract_id();
        }
    }

    //==========================================================================
    // FieldNames

    void encode(const logger::FieldNames &native, cc::logger::FieldNames *proto)
    {
        protobuf::assign_repeated(native, proto->mutable_field_names());
    }

    void decode(const cc::logger::FieldNames &proto, logger::FieldNames *native)
    {
        protobuf::assign_to_vector(proto.field_names(), native);
    }

}  // namespace protobuf
