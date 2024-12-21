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
        checkstatus(avro_generic_value_new(this->iface, &this->value));
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
            &schema));
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
            expected_name ? &field_name : nullptr));

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
            expected_index ? &field_index : nullptr));

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

    void CompoundValue::set_timestamp(avro_value_t *value,
                                      const core::dt::TimePoint &tp)
    {
        This::set_long(value, core::dt::to_milliseconds(tp));
    }

    void CompoundValue::set_duration(avro_value_t *value,
                                     const core::dt::Duration &dur)
    {
        This::set_long(value, core::dt::to_milliseconds(dur));
    }

    void CompoundValue::set_variant(avro_value_t *value,
                                    const core::types::Value &variant)
    {
        switch (variant.type())
        {
        case core::types::ValueType::NONE:
            This::set_null(value);
            break;

        case core::types::ValueType::BOOL:
            This::set_boolean(value, variant.as_bool());
            break;

        case core::types::ValueType::UINT:
            This::set_long(value, variant.as_uint64());
            break;

        case core::types::ValueType::SINT:
            This::set_long(value, variant.as_sint64());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            This::set_string(value, variant.as_string());
            break;

        case core::types::ValueType::REAL:
            This::set_double(value, variant.as_real());
            break;

        case core::types::ValueType::COMPLEX:
            This::set_complex(value, variant.as_complex());
            break;

        case core::types::ValueType::BYTEVECTOR:
            This::set_bytes(value, variant.get<core::types::ByteVector>());
            break;

        case core::types::ValueType::TIMEPOINT:
            This::set_timestamp(value, variant.as_timepoint());
            break;

        case core::types::ValueType::DURATION:
            This::set_duration(value, variant.as_duration());
            break;

        case core::types::ValueType::VALUELIST:
            This::set_variant_list(value, *variant.get_valuelist());
            break;

        // case core::types::ValueType::TVLIST:
        //     break;

        case core::types::ValueType::KVMAP:
            This::set_variant_map(value, *variant.get_kvmap());
            break;

        default:
            logf_notice("No known Avro conversions from value type %s (index %d)",
                        variant.type(),
                        variant.index());
            break;
        }
    }

    void CompoundValue::set_variant_list(avro_value_t *value,
                                         const core::types::ValueList &list)
    {
    }

    void CompoundValue::set_variant_map(avro_value_t *value,
                                        const core::types::KeyValueMap &kvmap)
    {
    }

}  // namespace avro
