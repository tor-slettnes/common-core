// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.c++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logging/message/message.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-datetime-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // Loggable

    void encode(const core::types::Loggable &native,
                cc::platform::multilogger::Loggable *proto) noexcept
    {
        if (auto *message = dynamic_cast<const core::logging::Message *>(&native))
        {
            encode(*message, proto->mutable_message());
        }
        else if (auto *data = dynamic_cast<const core::logging::Data *>(&native))
        {
            encode(*data, proto->mutable_data());
        }
        else if (auto *error = dynamic_cast<const core::status::Error *>(&native))
        {
            encode(*error, proto->mutable_error());
        }
    }

    core::types::Loggable::ptr decode_loggable(
        const cc::platform::multilogger::Loggable &proto,
        const std::string &default_host) noexcept
    {
        switch (proto.event_type_case())
        {
        case cc::platform::multilogger::Loggable::EventTypeCase::kMessage:
            return decoded_shared<core::logging::Message>(proto.message(), default_host);

        case cc::platform::multilogger::Loggable::EventTypeCase::kData:
            return decoded_shared<core::logging::Data>(proto.data());

        case cc::platform::multilogger::Loggable::EventTypeCase::kError:
            return decoded_shared<core::status::Error>(proto.error());

        default:
            return nullptr;
        }
    }

    //==========================================================================
    // Data

    void encode(const core::logging::Data &native,
                cc::platform::multilogger::Data *proto) noexcept
    {
        proto->set_contract_id(native.contract_id());
        encode(native.timepoint(), proto->mutable_timestamp());
        encode(native.attributes(), proto->mutable_attributes());
    }

    void decode(const cc::platform::multilogger::Data &proto,
                core::logging::Data *native) noexcept
    {
        *native = core::logging::Data(
            proto.contract_id(),
            decoded<core::dt::TimePoint>(proto.timestamp()),
            decoded<core::types::KeyValueMap>(proto.attributes()));
    }

    //==========================================================================
    // core::logging::Message encoding to/decoding from cc::status::Error

    void encode(const core::logging::Message &native,
                cc::platform::multilogger::Message *proto) noexcept
    {
        proto->set_text(native.text());
        encode(native.timepoint(), proto->mutable_timestamp());
        proto->set_level(encoded(native.level()));
        proto->set_host(native.host());
        proto->set_application(native.origin());
        proto->set_log_scope(native.scopename());
        proto->set_thread_id(native.thread_id());
        proto->set_thread_name(native.thread_name());
        proto->set_task_name(native.task_name());
        proto->set_source_path(native.path().string());
        proto->set_source_line(native.lineno());
        proto->set_function_name(native.function());
        encode(native.attributes(), proto->mutable_attributes());
    }

    void decode(const cc::platform::multilogger::Message &proto,
                const std::string &default_host,
                core::logging::Message *native) noexcept
    {
        core::status::Level level = decoded<core::status::Level>(proto.level());

        core::logging::Scope::ptr scope =
            !proto.log_scope().empty()
                ? core::logging::Scope::create(proto.log_scope(), level)
                : log_scope;

        std::string host =
            !proto.host().empty()
                ? proto.host()
                : default_host;

        *native = core::logging::Message(
            proto.text(),                                            // text
            level,                                                   // level
            scope,                                                   // scope
            decoded<core::dt::TimePoint>(proto.timestamp()),         // tp
            proto.source_path(),                                     // path
            proto.source_line(),                                     // lineno
            proto.function_name(),                                   // function
            proto.thread_id(),                                       // thread_id
            proto.thread_name(),                                     // thread_name
            proto.task_name(),                                       // task_name
            host,                                                    // host
            proto.application(),                                     // origin
            decoded<core::types::KeyValueMap>(proto.attributes()));  // attributes
    }

    //==========================================================================
    // SinkID

    void encode(const multilogger::SinkID &native, cc::platform::multilogger::SinkID *proto)
    {
        proto->set_sink_id(native);
    }

    void decode(const cc::platform::multilogger::SinkID &proto, multilogger::SinkID *native)
    {
        *native = proto.sink_id();
    }

    //==========================================================================
    // SinkSpec

    void encode(const multilogger::SinkSpec &native, cc::platform::multilogger::SinkSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_sink_type(native.sink_type);
        // proto->set_permanent(native.permanent);
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

    void decode(const cc::platform::multilogger::SinkSpec &proto, multilogger::SinkSpec *native)
    {
        native->sink_id = proto.sink_id();
        native->sink_type = proto.sink_type();
        // native->permanent = proto.permanent();
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
    // multilogger::SinkSpecs <-> cc::platform::multilogger::SinkSpecs

    void encode(const multilogger::SinkSpecs &native, cc::platform::multilogger::SinkSpecs *proto)
    {
        encode_vector(native, proto->mutable_specs());
    }

    void decode(const cc::platform::multilogger::SinkSpecs &proto, multilogger::SinkSpecs *native)
    {
        decode_to_vector(proto.specs(), native);
    }

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::platform::multilogger::ColumnSpec *proto)
    {
        proto->set_field_name(native.field_name);
        if (native.column_name)
        {
            proto->set_column_name(native.column_name.value());
        }
        proto->set_column_type(encoded<cc::platform::multilogger::ColumnType>(native.column_type));
    }

    void decode(const cc::platform::multilogger::ColumnSpec &proto, core::logging::ColumnSpec *native)
    {
        native->field_name = proto.field_name();
        if (proto.has_column_name())
        {
            native->column_name = proto.column_name();
        }
        decode(proto.column_type(), &native->column_type);
    }

    //==========================================================================
    // ColumnType

    static core::types::ValueMap<core::types::ValueType, cc::platform::multilogger::ColumnType> coltype_map = {
        {core::types::ValueType::NONE, cc::platform::multilogger::COLTYPE_NONE},
        {core::types::ValueType::BOOL, cc::platform::multilogger::COLTYPE_BOOL},
        {core::types::ValueType::SINT, cc::platform::multilogger::COLTYPE_INT},
        {core::types::ValueType::REAL, cc::platform::multilogger::COLTYPE_REAL},
        {core::types::ValueType::STRING, cc::platform::multilogger::COLTYPE_TEXT},
        {core::types::ValueType::BYTEVECTOR, cc::platform::multilogger::COLTYPE_BLOB},
        {core::types::ValueType::TIMEPOINT, cc::platform::multilogger::COLTYPE_DATETIME},
    };

    void encode(const core::types::ValueType &native, cc::platform::multilogger::ColumnType *proto)
    {
        *proto = coltype_map.get(native, cc::platform::multilogger::COLTYPE_NONE);
    }

    void decode(const cc::platform::multilogger::ColumnType &proto, core::types::ValueType *native)
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

    void encode(const multilogger::ListenerSpec &native, cc::platform::multilogger::ListenerSpec *proto)
    {
        proto->set_sink_id(native.sink_id);
        proto->set_min_level(encoded<cc::status::Level>(native.min_level));

        if (native.contract_id.has_value())
        {
            proto->set_contract_id(native.contract_id.value());
        }
    }

    void decode(const cc::platform::multilogger::ListenerSpec &proto, multilogger::ListenerSpec *native)
    {
        native->sink_id = proto.sink_id();
        decode(proto.min_level(), &native->min_level);

        if (proto.has_contract_id())
        {
            native->contract_id = proto.contract_id();
        }
    }
}  // namespace protobuf
