// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.c++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-datetime-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // SinkID

    void encode(const multilogger::SinkID &native, cc::multilogger::SinkID *proto)
    {
        proto->set_sink_id(native);
    }

    void decode(const cc::multilogger::SinkID &proto, multilogger::SinkID *native)
    {
        *native = proto.sink_id();
    }

    //==========================================================================
    // SinkSpec

    void encode(const multilogger::SinkSpec &native, cc::multilogger::SinkSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_sink_type(native.sink_type);
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

    void decode(const cc::multilogger::SinkSpec &proto, multilogger::SinkSpec *native)
    {
        native->sink_id = proto.sink_id();
        native->sink_type = proto.sink_type();
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
    // multilogger::SinkSpecs <-> cc::multilogger::SinkSpecs

    void encode(const multilogger::SinkSpecs &native, cc::multilogger::SinkSpecs *proto)
    {
        encode_vector(native, proto->mutable_specs());
    }

    void decode(const cc::multilogger::SinkSpecs &proto, multilogger::SinkSpecs *native)
    {
        decode_to_vector(proto.specs(), native);
    }

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::multilogger::ColumnSpec *proto)
    {
        proto->set_event_field(native.event_field);
        if (native.column_name)
        {
            proto->set_column_name(native.column_name.value());
        }
        proto->set_column_type(encoded<cc::multilogger::ColumnType>(native.column_type));
    }

    void decode(const cc::multilogger::ColumnSpec &proto, core::logging::ColumnSpec *native)
    {
        native->event_field = proto.event_field();
        if (proto.has_column_name())
        {
            native->column_name = proto.column_name();
        }
        decode(proto.column_type(), &native->column_type);
    }

    //==========================================================================
    // ColumnType

    static core::types::ValueMap<core::types::ValueType, cc::multilogger::ColumnType> coltype_map = {
        {core::types::ValueType::NONE, cc::multilogger::COLTYPE_NONE},
        {core::types::ValueType::BOOL, cc::multilogger::COLTYPE_BOOL},
        {core::types::ValueType::SINT, cc::multilogger::COLTYPE_INT},
        {core::types::ValueType::REAL, cc::multilogger::COLTYPE_REAL},
        {core::types::ValueType::STRING, cc::multilogger::COLTYPE_TEXT},
        {core::types::ValueType::BYTEVECTOR, cc::multilogger::COLTYPE_BLOB},
        {core::types::ValueType::TIMEPOINT, cc::multilogger::COLTYPE_DATETIME},
    };

    void encode(const core::types::ValueType &native, cc::multilogger::ColumnType *proto)
    {
        *proto = coltype_map.get(native, cc::multilogger::COLTYPE_NONE);
    }

    void decode(const cc::multilogger::ColumnType &proto, core::types::ValueType *native)
    {
        *native = core::types::ValueType::NONE;
        for (const auto &[native_candidate, proto_candidate] : coltype_map)
        {
            if (proto_candidate == proto)
            {
                *native = native_candidate;
            }
        }
    }

    //==========================================================================
    // ListenerSpec

    void encode(const multilogger::ListenerSpec &native, cc::multilogger::ListenerSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_min_level(encoded<cc::status::Level>(native.min_level));

        if (native.contract_id.has_value())
        {
            proto->set_contract_id(native.contract_id.value());
        }
    }

    void decode(const cc::multilogger::ListenerSpec &proto, multilogger::ListenerSpec *native)
    {
        native->sink_id = proto.sink_id();
        decode(proto.min_level(), &native->min_level);

        if (proto.has_contract_id())
        {
            native->contract_id = proto.contract_id();
        }
    }
}  // namespace protobuf
