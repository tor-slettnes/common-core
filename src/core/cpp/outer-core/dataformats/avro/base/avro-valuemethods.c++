/// -*- c++ -*-
//==============================================================================
/// @file avro-valuemethods.c++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes
//==============================================================================

#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

// #include <stdlib.h>

// #include <algorithm>
// #include <type_traits>

namespace avro
{
    avro_schema_t schema(const avro_value_t &avro_value)
    {
        return avro_value_get_schema(&avro_value);
    }

    avro_type_t type(const avro_value_t &avro_value)
    {
        return avro_value_get_type(&avro_value);
    }

    std::string type_name(const avro_value_t &avro_value)
    {
        return avro_schema_type_name(avro::schema(avro_value));
    }

    avro_schema_t schema_from_json(
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

    avro_value_t get_field_by_index(
        const avro_value_t &avro_value,
        int index,
        const std::optional<std::string> &expected_name)
    {
        avro_value_t indexed_value;
        const char *field_name = nullptr;
        checkstatus(avro_value_get_by_index(
                        &avro_value,
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

    avro_value_t get_field_by_name(
        const avro_value_t &avro_value,
        const std::string &name,
        const std::optional<size_t> &expected_index)
    {
        avro_value_t named_value;
        size_t field_index = 0;
        checkstatus(avro_value_get_by_name(
                        &avro_value,
                        name.c_str(),
                        &named_value,
                        expected_index ? &field_index : nullptr),
                    "avro_value_get_by_name");

        if (expected_index.value_or(0) != field_index)
        {
            throw core::exception::NotFound(
                "Mismatched Avro field index for specified name",
                core::types::KeyValueMap({
                    {"name", name},
                    {"expected", expected_index.value()},
                    {"actual", field_index},
                }));
        }

        return named_value;
    }

    void set_null(avro_value_t *avro_value)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_null(avro_value));
    }

    void set_int(
        avro_value_t *avro_value,
        int intvalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_int(avro_value, intvalue));
    }

    void set_long(
        avro_value_t *avro_value,
        long longvalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_long(avro_value, longvalue));
    }

    void set_float(
        avro_value_t *avro_value,
        float floatvalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_float(avro_value, floatvalue));
    }

    void set_double(
        avro_value_t *avro_value,
        double doublevalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_double(avro_value, doublevalue));
    }

    void set_boolean(
        avro_value_t *avro_value,
        bool boolvalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_boolean(avro_value, boolvalue));
    }

    void set_enum(
        avro_value_t *avro_value,
        int enumvalue)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_enum(avro_value, enumvalue));
    }

    void set_enum(
        avro_value_t *avro_value,
        const std::string &enumsymbol)
    {
        int enum_value = avro_schema_enum_get_by_name(
            avro::schema(*avro_value),
            enumsymbol.data());

        checkstatus(
            enum_value,
            core::str::format("avro_schema_enum_get_by_name(%s)", enumsymbol));

        avro::set_enum(avro_value, enum_value);
    }

    void set_string(
        avro_value_t *avro_value,
        const std::string &string)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_string_len(
            avro_value,
            string.c_str(),
            string.size() + 1));  // Length needs to include trailing null terminator
    }

    void set_bytes(
        avro_value_t *avro_value,
        const core::types::Bytes &bytes)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_bytes(
            avro_value,
            const_cast<core::types::Byte *>(bytes.data()),
            bytes.size()));
    }

    void set_bytes(
        avro_value_t *avro_value,
        const std::string &bytes)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_bytes(
            avro_value,
            const_cast<char *>(bytes.data()),
            bytes.size()));
    }

    void set_fixed(
        avro_value_t *avro_value,
        const core::types::Bytes &bytes,
        const std::optional<std::size_t> &nbytes)
    {
        avro_value_reset(avro_value);
        checkstatus(avro_value_set_fixed(
            avro_value,
            const_cast<core::types::Byte *>(bytes.data()),
            nbytes.value_or(bytes.size())));
    }

    void set_datetime_interval(
        avro_value_t *value,
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
        avro::set_fixed(value, packed_duration);
    }

    void set_time_interval(
        avro_value_t *avro_value,
        const core::dt::Duration &dur)
    {
        avro::set_long(avro_value, core::dt::to_milliseconds(dur));
    }

    void set_timestamp(
        avro_value_t *avro_value,
        const core::dt::TimePoint &tp)
    {
        avro::set_long(avro_value, core::dt::to_milliseconds(tp));
    }

    void set_variant(avro_value_t *value,
                     const core::types::Value &variant)
    {
        avro_value_t value_field = avro::get_field_by_index(*value, 0, SchemaField_VariantValue);
        avro_value_t branch;
        switch (variant.type())
        {
        case core::types::ValueType::NONE:
            avro::set_variant_branch(&value_field, VariantSchema::VT_NULL, &branch);
            avro::set_null(&branch);
            break;

        case core::types::ValueType::BOOL:
            avro::set_variant_branch(&value_field, VariantSchema::VT_BOOL, &branch);
            avro::set_boolean(&branch, variant.as_bool());
            break;

        case core::types::ValueType::UINT:
        case core::types::ValueType::SINT:
            avro::set_variant_branch(&value_field, VariantSchema::VT_LONG, &branch);
            avro::set_long(&branch, variant.as_sint64());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            avro::set_variant_branch(&value_field, VariantSchema::VT_STRING, &branch);
            avro::set_string(&branch, variant.as_string());
            break;

        case core::types::ValueType::REAL:
            avro::set_variant_branch(&value_field, VariantSchema::VT_DOUBLE, &branch);
            avro::set_double(&branch, variant.as_real());
            break;

        case core::types::ValueType::BYTEVECTOR:
            avro::set_variant_branch(&value_field, VariantSchema::VT_BYTES, &branch);
            avro::set_bytes(&branch, variant.get<core::types::ByteVector>());
            break;

        case core::types::ValueType::TIMEPOINT:
            // avro::set_variant_branch(&value_field, VariantSchema::VT_TIMESTAMP, &branch);
            avro::set_variant_branch(&value_field, VariantSchema::VT_LONG, &branch);
            avro::set_timestamp(&branch, variant.as_timepoint());
            break;

        case core::types::ValueType::DURATION:
            // avro::set_variant_branch(&value_field, VariantSchema::VT_INTERVAL, &branch);
            avro::set_variant_branch(&value_field, VariantSchema::VT_LONG, &branch);
            avro::set_time_interval(&branch, variant.as_duration());
            break;

            // case core::types::ValueType::VALUELIST:
            //     avro::set_variant_branch(&value_field, VariantSchema::VT_ARRAY, &branch);
            //     avro::set_variant_list(&branch, variant.get_valuelist());
            //     break;

            // case core::types::ValueType::KVMAP:
            //     avro::set_variant_branch(&value_field, VariantSchema::VT_MAP, &branch);
            //     avro::set_variant_map(&branch, variant.get_kvmap());
            //     break;

        default:
            avro::set_variant_branch(&value_field, VariantSchema::VT_NULL, &branch);
            avro::set_null(&branch);
            logf_notice("No known Avro conversions from variant type %s: %r",
                        variant.type(),
                        variant);
            break;
        }
    }

    void set_variant_list(avro_value_t *avro_value,
                          const core::types::ValueList &list)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_ARRAY,
                "Attempt to assign variant value list to Avro non-array value type %s",
                avro::type_name(*avro_value));

        for (const core::types::Value &value : list)
        {
            avro_value_t element;
            checkstatus(avro_value_append(avro_value,  // value
                                          &element,    // child
                                          nullptr),    // new_index
                        "avro_value_append");
            avro::set_variant(&element, value);
        }
    }

    void set_variant_map(avro_value_t *avro_value,
                         const core::types::KeyValueMap &kvmap)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_MAP,
                "Attempt to assign variant key value map to Avro non-map value type %s",
                avro::type_name(*avro_value));

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

            avro::set_variant(&element, value);
        }
    }

    void set_variant_branch(avro_value_t *avro_value,
                            VariantSchema::Type type,
                            avro_value_t *branch)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_UNION,
                "Attempt to set branch of Avro non-union value type %s",
                avro::type_name(*avro_value));

        avro_value_reset(avro_value);
        checkstatus(avro_value_set_branch(
                        avro_value,              // value
                        static_cast<int>(type),  // discriminant
                        branch),                 // branch
                    core::str::format("set_variant_branch (%d)", type));
    }

    void set_value(avro_value_t *avro_value,
                   const core::types::Value &value)
    {
        switch (avro::type(*avro_value))
        {
        case AVRO_STRING:
        case AVRO_LINK:
            avro::set_string(avro_value, value.get_string());
            break;

        case AVRO_BYTES:
            avro::set_bytes(avro_value, value.as_bytevector());
            break;

        case AVRO_INT32:
            avro::set_int(avro_value, value.as_sint32());
            break;

        case AVRO_INT64:
            avro::set_long(avro_value, value.as_sint64());
            break;

        case AVRO_FLOAT:
            avro::set_float(avro_value, value.as_float());
            break;

        case AVRO_DOUBLE:
            avro::set_double(avro_value, value.as_double());
            break;

        case AVRO_BOOLEAN:
            avro::set_boolean(avro_value, value.as_bool());
            break;

        case AVRO_NULL:
            avro::set_null(avro_value);
            break;

        case AVRO_ENUM:
            if (value.is_numeric())
            {
                avro::set_enum(avro_value, value.as_uint());
            }
            else
            {
                avro::set_enum(avro_value, value.as_string());
            }
            break;

        case AVRO_FIXED:
            avro::set_fixed(avro_value, value.get_bytevector());
            break;

        case AVRO_MAP:
            avro::set_map(avro_value, value.get_kvmap());
            break;

        case AVRO_RECORD:
            avro::set_record(avro_value, value.get_kvmap());
            break;

        case AVRO_ARRAY:
            avro::set_array(avro_value, value.get_valuelist());
            break;

        case AVRO_UNION:
            avro::set_union(avro_value, value);
            break;

        default:
            break;
        }
    }

    void set_map(avro_value_t *avro_value,
                 const core::types::KeyValueMap &kvmap)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_MAP,
                "Attempt to assign key value map to Avro non-map value type %s",
                avro::type_name(*avro_value));

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
            avro::set_value(&element, value);
        }
    }

    void set_array(avro_value_t *avro_value,
                   const core::types::ValueList &values)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_ARRAY,
                "Attempt to assign value list to Avro non-array value type %s",
                avro::type_name(*avro_value));

        for (const core::types::Value &value : values)
        {
            avro_value_t element;
            checkstatus(avro_value_append(avro_value,  // value
                                          &element,    // child
                                          nullptr),    // new_index
                        "avro_value_append");
            avro::set_value(&element, value);
        }
    }

    void set_record(avro_value_t *avro_value,
                    const core::types::KeyValueMap &kvmap)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_RECORD,
                "Attempt to assign key value map to Avro non-record value type %s",
                avro::type_name(*avro_value));

        size_t size = 0;
        checkstatus(avro_value_get_size(avro_value, &size));

        for (int field_num = 0; field_num < size; field_num++)
        {
            avro_value_t field_value;
            const char *field_name = nullptr;
            checkstatus(avro_value_get_by_index(
                            avro_value,
                            field_num,
                            &field_value,
                            &field_name),
                        "avro_value_get_by_index");

            avro::set_value(&field_value, kvmap.get(field_name));
        }
    }

    // void set_field_value(
    //     avro_value_t *avro_value,
    //     const std::string &field_name,
    //     const core::types::Value &value)
    // {
    //     assertf(avro_value_get_type(avro_value) == AVRO_RECORD,
    //             "Attempt to assign variant key value map to Avro non-record value type %s",
    //             avro::type_name(avro_value));

    //     avro_value_t field_value;
    //     checkstatus(avro_value_get_by_name(
    //         avro_value,
    //         field_name.c_str(),
    //         &field_value,
    //         nullptr));
    //     avro::set_value(&field_value, value);
    // }

    // void set_field_value(
    //     avro_value_t *avro_value,
    //     const std::uint64_t &field_index,
    //     const core::types::Value &value)
    // {
    //     assertf(avro_value_get_type(avro_value) == AVRO_RECORD,
    //             "Attempt to assign variant key value map to Avro non-record value type %s",
    //             avro::type_name(avro_value));

    //     avro_value_t field_value;
    //     checkstatus(avro_value_get_by_index(
    //         avro_value,
    //         field_index,
    //         &field_value,
    //         nullptr));
    //     avro::set_value(&field_value, value);
    // }

    void set_union(avro_value_t *avro_value,
                   const core::types::Value &value)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_UNION,
                "Attempt to assign union to Avro non-union value type %s",
                avro::type_name(*avro_value));

        avro_schema_t avro_schema = avro::schema(*avro_value);
        size_t union_size = avro_schema_union_size(avro_schema);
        std::optional<int> discriminant;
        bool hit = false;
        avro_value_reset(avro_value);

        for (int index = 0; index < union_size; index++)
        {
            avro_type_t type = avro_schema_union_branch(avro_schema, index)->type;
            switch (type)
            {
            case AVRO_STRING:
            case AVRO_LINK:
                hit = value.empty();
                break;

            case AVRO_BYTES:
            case AVRO_FIXED:
                hit = value.is_bytevector();
                break;

            case AVRO_INT32:
            case AVRO_INT64:
                hit = value.is_integral();
                break;

            case AVRO_FLOAT:
            case AVRO_DOUBLE:
                hit = value.is_real();
                break;

            case AVRO_BOOLEAN:
                hit = value.is_bool();
                break;

            case AVRO_ENUM:
                hit = value.is_integral() || value.is_string();
                break;

            case AVRO_RECORD:
            case AVRO_MAP:
                hit = value.is_mappable();
                break;

            case AVRO_ARRAY:
                hit = value.is_sequence();
                break;

            case AVRO_UNION:
                hit = false;
                break;

            case AVRO_NULL:
                hit = value.empty();
                break;
            }

            if (hit)
            {
                avro_value_t branch;
                checkstatus(avro_value_set_branch(
                    avro_value,
                    index,
                    &branch));
                avro::set_value(&branch, value);
                break;
            }
        }

        if (!hit)
        {
            throwf(core::exception::InvalidArgument,
                   "Avro union %s does not accept value type %s",
                   avro::type_name(*avro_value),
                   value.type_name());
        }
    }

    void set_from_serialized(avro_value_t *value,
                             const core::types::Bytes &bytes)
    {
        avro_reader_t reader = avro_reader_memory(
            reinterpret_cast<const char *>(bytes.data()), bytes.size());

        checkstatus(avro_value_read(reader, value));
    }

    std::optional<int> get_int(
        const avro_value_t &avro_value)
    {
        int result = 0;
        if (avro_value_get_int(&avro_value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<long> get_long(
        const avro_value_t &avro_value)
    {
        long result = 0;
        if (avro_value_get_long(&avro_value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<float> get_float(
        const avro_value_t &avro_value)
    {
        float result = 0.0;
        if (avro_value_get_float(&avro_value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<double> get_double(
        const avro_value_t &avro_value)
    {
        double result = 0.0;
        if (avro_value_get_double(&avro_value, &result) == 0)
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<bool> get_boolean(
        const avro_value_t &avro_value)
    {
        int result = 0;
        if (avro_value_get_boolean(&avro_value, &result))
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<int> get_enum_value(
        const avro_value_t &avro_value)
    {
        int result = 0;
        if (avro_value_get_enum(&avro_value, &result))
        {
            return result;
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> get_enum_symbol(
        const avro_value_t &avro_value)
    {
        int enum_value = 0;
        avro_schema_t schema = avro::schema(avro_value);

        if ((avro_value_get_enum(&avro_value, &enum_value) == 0) &&
            (enum_value < avro_schema_enum_number_of_symbols(schema)))
        {
            return avro_schema_enum_get(schema, enum_value);
        }
        else
        {
            return {};
        }
    }

    std::optional<std::string> get_string(
        const avro_value_t &avro_value)
    {
        const char *c_string = nullptr;
        std::size_t size = 0;

        if (avro_value_get_string(&avro_value, &c_string, &size) == 0)
        {
            if (size > 0)
            {
                return std::string(c_string, size - 1);
            }
        }
        return {};
    }

    std::optional<core::types::ByteVector> get_bytes(
        const avro_value_t &avro_value)
    {
        const core::types::Byte *bytes = nullptr;
        std::size_t size = 0;
        if (avro_value_get_bytes(
                &avro_value,
                reinterpret_cast<const void **>(&bytes),
                &size) == 0)
        {
            return core::types::ByteVector(bytes, bytes + size);
        }
        else
        {
            return {};
        }
    }

    std::optional<core::types::ByteVector> get_fixed(
        const avro_value_t &avro_value)
    {
        const core::types::Byte *data = nullptr;
        std::size_t size = 0;
        if (avro_value_get_fixed(
                &avro_value,
                reinterpret_cast<const void **>(&data),
                &size) == 0)
        {
            return core::types::ByteVector(data, data + size);
        }
        else
        {
            return {};
        }
    }

    core::types::Value get_value(
        const avro_value_t &avro_value,
        bool enums_as_strings)
    {
        switch (avro::type(avro_value))
        {
        case AVRO_STRING:
        case AVRO_LINK:
            return avro::get_string(avro_value);

        case AVRO_BYTES:
            return avro::get_bytes(avro_value);

        case AVRO_INT32:
            return avro::get_int(avro_value);

        case AVRO_INT64:
            return avro::get_long(avro_value);

        case AVRO_FLOAT:
            return avro::get_float(avro_value);

        case AVRO_DOUBLE:
            return avro::get_double(avro_value);

        case AVRO_BOOLEAN:
            return avro::get_boolean(avro_value);

        case AVRO_NULL:
            return {};

        case AVRO_ENUM:
            if (enums_as_strings)
            {
                return avro::get_enum_symbol(avro_value);
            }
            else
            {
                return avro::get_enum_value(avro_value);
            }

        case AVRO_FIXED:
            return avro::get_fixed(avro_value);

        case AVRO_MAP:
        case AVRO_RECORD:
            return avro::get_map(avro_value, enums_as_strings);

        case AVRO_ARRAY:
            return avro::get_array(avro_value, enums_as_strings);

        case AVRO_UNION:
            return avro::get_union(avro_value, enums_as_strings);

        default:
            return {};
        }
    }

    core::types::KeyValueMapPtr get_map(
        const avro_value_t &avro_value,
        bool enums_as_strings)
    {
        std::size_t size = 0;
        core::types::KeyValueMapPtr kvmap;
        if (avro_value_get_size(&avro_value, &size) == 0)
        {
            kvmap = std::make_shared<core::types::KeyValueMap>();
            for (std::size_t index = 0; index < size; index++)
            {
                const char *key = nullptr;
                avro_value_t avro_value;
                avro_value_get_by_index(
                    &avro_value,
                    index,
                    &avro_value,
                    &key);

                if (key != nullptr)
                {
                    kvmap->insert_or_assign(
                        key,
                        get_value(avro_value, enums_as_strings));
                }
            }
        }
        return kvmap;
    }

    core::types::ValueListPtr get_array(
        const avro_value_t &avro_value,
        bool enums_as_strings)
    {
        std::size_t size = 0;
        core::types::ValueListPtr list;
        if (avro_value_get_size(&avro_value, &size) == 0)
        {
            list = std::make_shared<core::types::ValueList>(size);

            for (std::size_t index = 0; index < size; index++)
            {
                avro_value_t avro_value;
                checkstatus(
                    avro_value_get_by_index(
                        &avro_value,
                        index,
                        &avro_value,
                        nullptr));

                list->at(index) = get_value(avro_value, enums_as_strings);
            }
        }
        return list;
    }

    core::types::Value get_union(
        const avro_value_t &avro_value,
        bool enums_as_strings)
    {
        avro_value_t branch_value;
        if (avro_value_get_current_branch(&avro_value, &branch_value) == 0)
        {
            return avro::get_value(branch_value, enums_as_strings);
        }
        else
        {
            return {};
        }
    }

    std::set<avro_type_t> union_types(
        const avro_schema_t &schema)
    {
        std::set<avro_type_t> types;

        size_t union_size = avro_schema_union_size(schema);
        for (int index = 0; index < union_size; index++)
        {
            types.insert(avro_schema_union_branch(schema, index)->type);
        }
        return types;
    }
}  // namespace avro
