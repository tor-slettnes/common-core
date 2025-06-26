/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.c++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-compoundvalue.h++"
#include "avro-status.h++"
#include "chrono/date-time.h++"
#include "parsers/json/writer.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include <iostream>

namespace avro
{

    CompoundValue::CompoundValue(avro_schema_t schema)
        : BaseValue(),
          schema(schema),
          iface(checkstatus(avro_generic_class_from_schema(schema)))
    {
        checkstatus(avro_generic_value_new(this->iface, &this->value),
                    "avro_generic_value_new");
    }

    CompoundValue::CompoundValue(const std::string &json_schema)
        : CompoundValue(This::schema_from_json(json_schema))
    {
    }

    CompoundValue::CompoundValue(SchemaWrapper &&wrapper)
        : CompoundValue(wrapper.as_avro_schema())
    {
    }

    CompoundValue::~CompoundValue()
    {
        avro_value_iface_decref(this->iface);
        avro_schema_decref(schema);
    }

    avro_schema_t CompoundValue::schema_from_json(
        const std::string &json)
    {
        avro_schema_t schema;
        checkstatus(avro_schema_from_json_length(
                        json.data(),
                        json.size(),
                        &schema),
                    "avro_schema_from_json_length");
        return schema;
    }

    avro_value_t CompoundValue::get_by_index(
        avro_value_t *value,
        int index,
        const std::optional<std::string> &expected_name)
    {
        avro_value_t indexed_value;
        const char *field_name = nullptr;
        checkstatus(avro_value_get_by_index(
                        value,
                        index,
                        &indexed_value,
                        expected_name ? &field_name : nullptr),
                    "avro_value_get_by_index");

        if ((field_name != nullptr) && (field_name != expected_name.value()))
        {
            throw core::exception::NotFound(
                "Mismatched Avro field name at specified index",
                core::types::KeyValueMap({
                    {"index", index},
                    {"expected", expected_name.value()},
                    {"actual", field_name},
                }));
        }

        return indexed_value;
    }

    avro_value_t CompoundValue::get_by_name(
        avro_value_t *value,
        const std::string &name,
        const std::optional<size_t> &expected_index)
    {
        avro_value_t named_value;
        size_t field_index = 0;
        checkstatus(avro_value_get_by_name(
                        value,
                        name.c_str(),
                        &named_value,
                        expected_index ? &field_index : nullptr),
                    "avro_value_get_by_name");

        if (expected_index.value_or(0) != field_index)
        {
            throw core::exception::NotFound(
                "Mismatched Avro field index for sepcified name",
                core::types::KeyValueMap({
                    {"name", name},
                    {"expected", expected_index.value()},
                    {"actual", field_index},
                }));
        }

        return named_value;
    }

    void CompoundValue::set_enum_value(avro_value_t *value,
                                       std::uint64_t index,
                                       std::int64_t number)
    {
        avro_value_t enum_index = This::get_by_index(value, 0, SchemaField_EnumIndex);
        This::set_enum(&enum_index, index);

        avro_value_t enum_value = This::get_by_index(value, 1, SchemaField_EnumValue);
        This::set_long(&enum_index, number);
    }

    void CompoundValue::set_datetime_interval(avro_value_t *value,
                                              const core::dt::DateTimeInterval &interval)
    {
        // The Avro `duration` logical type is a fixed array of 12 bytes, split
        // into three groups of packed 4-byte (32-bit) unsigned integers with
        // little endian byte ordering.

        std::uint32_t months = 0;
        std::uint32_t days = 0;
        std::uint32_t ms = 0;

        switch (interval.unit)
        {
        case core::dt::TimeUnit::NANOSECOND:
            ms = interval.count / 1000000;
            break;

        case core::dt::TimeUnit::MICROSECOND:
            ms = interval.count / 1000;
            break;

        case core::dt::TimeUnit::MILLISECOND:
            ms = interval.count;
            break;

        case core::dt::TimeUnit::SECOND:
            ms = interval.count * 1000;
            break;

        case core::dt::TimeUnit::MINUTE:
            ms = interval.count * 1000 * 60;
            break;

        case core::dt::TimeUnit::HOUR:
            ms = interval.count * 1000 * 60 * 60;
            break;

        case core::dt::TimeUnit::DAY:
            days = interval.count;
            break;

        case core::dt::TimeUnit::WEEK:
            days = interval.count * 7;
            break;

        case core::dt::TimeUnit::MONTH:
            months = interval.count;
            break;

        case core::dt::TimeUnit::YEAR:
            months = interval.count * 12;
            break;
        }

        std::vector<std::uint32_t> counts = {months, days, ms};

        core::types::ByteVector packed_duration;
        packed_duration.reserve(LogicalType_Duration_Size);

        for (std::uint32_t count : counts)
        {
            for (uint byte = 0; byte < sizeof(std::uint32_t); byte++)
            {
                packed_duration.push_back((count >> (8 * byte)) & 0xFF);
            }
        }
        This::set_fixed(value, packed_duration);
    }

    void CompoundValue::set_time_interval(avro_value_t *value,
                                          const core::dt::Duration &dur)
    {
        This::set_long(value, core::dt::to_milliseconds(dur));
    }

    // void CompoundValue::set_time_interval(avro_value_t *value,
    //                                       const core::dt::Duration &dur)
    // {
    //     auto seconds = std::chrono::duration_cast<std::chrono::seconds>(dur);
    //     auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(dur) -
    //                  std::chrono::duration_cast<std::chrono::nanoseconds>(seconds);

    //     avro_value_t seconds_value = This::get_by_index(value, 0, SchemaField_TimeSeconds);
    //     This::set_long(&seconds_value, seconds.count());

    //     avro_value_t nanos_value = This::get_by_index(value, 1, SchemaField_TimeNanos);
    //     This::set_int(&nanos_value, nanos.count());
    // }

    void CompoundValue::set_timestamp(avro_value_t *value,
                                      const core::dt::TimePoint &tp)
    {
        // This::set_time_interval(value, tp.time_since_epoch());
        This::set_long(value, core::dt::to_milliseconds(tp));
    }

    void CompoundValue::set_variant(avro_value_t *value,
                                    const core::types::Value &variant)
    {
        avro_value_t value_field = This::get_by_index(value, 0, SchemaField_VariantValue);
        avro_value_t branch;
        switch (variant.type())
        {
        case core::types::ValueType::NONE:
            This::set_branch(&value_field, VariantSchema::VT_NULL, &branch);
            This::set_null(&branch);
            break;

        case core::types::ValueType::BOOL:
            This::set_branch(&value_field, VariantSchema::VT_BOOL, &branch);
            This::set_boolean(&branch, variant.as_bool());
            break;

        case core::types::ValueType::UINT:
        case core::types::ValueType::SINT:
            This::set_branch(&value_field, VariantSchema::VT_LONG, &branch);
            This::set_long(&branch, variant.as_sint64());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            This::set_branch(&value_field, VariantSchema::VT_STRING, &branch);
            This::set_string(&branch, variant.as_string());
            break;

        case core::types::ValueType::REAL:
            This::set_branch(&value_field, VariantSchema::VT_DOUBLE, &branch);
            This::set_double(&branch, variant.as_real());
            break;

        case core::types::ValueType::BYTEVECTOR:
            This::set_branch(&value_field, VariantSchema::VT_BYTES, &branch);
            This::set_bytes(&branch, variant.get<core::types::ByteVector>());
            break;

        case core::types::ValueType::TIMEPOINT:
            // This::set_branch(&value_field, VariantSchema::VT_TIMESTAMP, &branch);
            This::set_branch(&value_field, VariantSchema::VT_LONG, &branch);
            This::set_timestamp(&branch, variant.as_timepoint());
            break;

        case core::types::ValueType::DURATION:
            // This::set_branch(&value_field, VariantSchema::VT_INTERVAL, &branch);
            This::set_branch(&value_field, VariantSchema::VT_LONG, &branch);
            This::set_time_interval(&branch, variant.as_duration());
            break;

            // case core::types::ValueType::VALUELIST:
            //     This::set_branch(&value_field, VariantSchema::VT_ARRAY, &branch);
            //     This::set_variant_list(&branch, *variant.get_valuelist());
            //     break;

            // case core::types::ValueType::KVMAP:
            //     This::set_branch(&value_field, VariantSchema::VT_MAP, &branch);
            //     This::set_variant_map(&branch, *variant.get_kvmap());
            //     break;

        default:
            This::set_branch(&value_field, VariantSchema::VT_NULL, &branch);
            This::set_null(&branch);
            logf_notice("No known Avro conversions from variant type %s: %r",
                        variant.type(),
                        variant);
            break;
        }
    }

    void CompoundValue::set_variant_list(avro_value_t *avro_value,
                                         const core::types::ValueList &list)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_ARRAY,
                "Attempt to assign variant value list to Avro non-array value type %s",
                This::type_name(avro_value));

        for (const core::types::Value &value : list)
        {
            avro_value_t element;
            checkstatus(avro_value_append(avro_value,  // value
                                          &element,    // child
                                          nullptr),    // new_index
                        "avro_value_append");
            This::set_variant(&element, value);
        }
    }

    void CompoundValue::set_variant_map(avro_value_t *avro_value,
                                        const core::types::KeyValueMap &kvmap)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_MAP,
                "Attempt to assign variant value list to Avro non-map value type %s",
                This::type_name(avro_value));

        for (const auto &[key, value] : kvmap)
        {
            avro_value_t element;
            checkstatus(avro_value_add(
                            avro_value,   // value
                            key.c_str(),  // key
                            &element,     // child
                            nullptr,      // index
                            nullptr),     // is_new
                        "avro_value_add");

            This::set_variant(&element, value);
        }
    }

    void CompoundValue::set_branch(avro_value_t *avro_value,
                                   VariantSchema::Type type,
                                   avro_value_t *branch)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_UNION,
                "Attempt to set branch of Avro non-union value type %s",
                This::type_name(avro_value));

        checkstatus(avro_value_set_branch(
                        avro_value,              // value
                        static_cast<int>(type),  // discriminant
                        branch),                 // branch
                    core::str::format("set_branch (%d)", type));
    }

}  // namespace avro
