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

    CompoundValue::CompoundValue(const SchemaWrapper &wrapper)
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

    void CompoundValue::set_complex(avro_value_t *value,
                                    const core::types::complex &complexvalue)
    {
        avro_value_t real = This::get_by_index(value, 0, SchemaField_ComplexReal);
        This::set_double(&real, complexvalue.real());

        avro_value_t imag = This::get_by_index(value, 0, SchemaField_ComplexImaginary);
        This::set_double(&imag, complexvalue.imag());
    }

    void CompoundValue::set_duration(avro_value_t *value,
                                     const core::dt::Duration &dur)
    {
        // The Avro `duration` logical type is a fixed array of 12 bytes, split
        // into three groups of packed 4-byte (32-bit) unsigned integers with
        // little endian byte ordering.

        constexpr std::uint32_t ms_per_day = (1000 * 60 * 60 * 24);  // approximate
        constexpr std::uint32_t days_per_month = 30;                 // approximate

        std::uint64_t total_ms = core::dt::to_milliseconds(dur);
        std::uint64_t total_days = total_ms / ms_per_day;
        std::uint32_t months = total_days / days_per_month;

        std::vector<std::uint32_t> counts = {
            months,
            static_cast<std::uint32_t>(total_days % days_per_month),
            static_cast<std::uint32_t>(total_ms % ms_per_day),
        };

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

    void CompoundValue::set_timestamp(avro_value_t *value,
                                      const core::dt::TimePoint &tp)
    {
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
            This::set_branch(&value_field, VariantSchema::VT_TIMESTAMP, &branch);
            This::set_timestamp(&branch, variant.as_timepoint());
            break;

        case core::types::ValueType::DURATION:
            This::set_branch(&value_field, VariantSchema::VT_DURATION, &branch);
            This::set_duration(&branch, variant.as_duration());
            break;

        case core::types::ValueType::VALUELIST:
            This::set_branch(&value_field, VariantSchema::VT_ARRAY, &branch);
            This::set_variant_list(&branch, *variant.get_valuelist());
            break;

            // case core::types::ValueType::TVLIST:
            //     break;

        case core::types::ValueType::KVMAP:
            This::set_branch(&value_field, VariantSchema::VT_MAP, &branch);
            This::set_variant_map(&branch, *variant.get_kvmap());
            break;

        default:
            logf_notice("No known Avro conversions from value type %s (index %d)",
                        variant.type(),
                        variant.index());
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

    void CompoundValue::set_branch(avro_value_t *value,
                                   VariantSchema::Type type,
                                   avro_value_t *branch)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_UNION,
                "Attempt to set branch of Avro non-union value type %s",
                This::type_name(avro_value));

        checkstatus(avro_value_set_branch(
                        value,
                        static_cast<int>(type),
                        branch),
                    core::str::format("set_branch (%d)", type));
    }

}  // namespace avro
