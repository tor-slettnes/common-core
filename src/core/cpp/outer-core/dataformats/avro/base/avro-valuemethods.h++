/// -*- c++ -*-
//==============================================================================
/// @file avro-valuemethods.h++
/// @brief Avro value wrapper - base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-schemabuilder.h++"
#include "types/value.h++"

#include <cstdint>
// #include <memory>
// #include <stdexcept>

namespace avro
{
    avro_schema_t schema(const avro_value_t &value);
    avro_type_t type(const avro_value_t &value);
    std::string type_name(const avro_value_t &value);

    avro_value_t get_field_by_index(
        const avro_value_t &value,
        int index,
        const std::optional<std::string> &expected_name = {});

    avro_value_t get_field_by_name(
        const avro_value_t &value,
        const std::string &name,
        const std::optional<std::size_t> &expected_index = {});

    avro_schema_t schema_from_json(
        const std::string &json);

    void set_null(avro_value_t *avro_value);

    void set_int(
        avro_value_t *avro_value,
        int intvalue);

    void set_long(
        avro_value_t *avro_value,
        long longvalue);

    void set_float(
        avro_value_t *avro_value,
        float floatvalue);

    void set_double(
        avro_value_t *avro_value,
        double doublevalue);

    void set_boolean(
        avro_value_t *avro_value,
        bool boolvalue);

    void set_enum(
        avro_value_t *avro_value,
        int enumvalue);

    void set_enum(
        avro_value_t *avro_value,
        const std::string &enumsymbol);

    void set_string(
        avro_value_t *avro_value,
        const std::string &string);

    void set_bytes(
        avro_value_t *avro_value,
        const core::types::Bytes &bytes);

    void set_bytes(
        avro_value_t *avro_value,
        const std::string &bytes);

    void set_fixed(
        avro_value_t *avro_value,
        const core::types::Bytes &bytes,
        const std::optional<std::size_t> &nbytes = {});

    void set_datetime_interval(
        avro_value_t *value,
        const core::dt::DateTimeInterval &interval);

    void set_time_interval(
        avro_value_t *value,
        const core::dt::Duration &dur);

    void set_timestamp(
        avro_value_t *value,
        const core::dt::TimePoint &tp);

    void set_variant(
        avro_value_t *value,
        const core::types::Value &variant);

    void set_variant_list(
        avro_value_t *value,
        const core::types::ValueList &list);

    void set_variant_map(
        avro_value_t *value,
        const core::types::KeyValueMap &kvmap);

    void set_variant_branch(
        avro_value_t *value,
        VariantSchema::Type type,
        avro_value_t *branch);

    void set_value(
        avro_value_t *avro_value,
        const core::types::Value &value);

    void set_map(
        avro_value_t *avro_value,
        const core::types::KeyValueMap &kvmap);

    void set_array(
        avro_value_t *avro_value,
        const core::types::ValueList &valuelist);

    void set_record(
        avro_value_t *avro_value,
        const core::types::KeyValueMap &kvmap);

    // void set_field_value(
    //     avro_value_t *avro_value,
    //     const std::string &field_name,
    //     const core::types::Value &value);

    // void set_field_value(
    //     avro_value_t *avro_value,
    //     const std::uint64_t &field_index,
    //     const core::types::Value &value);

    void set_union(
        avro_value_t *avro_value,
        const core::types::Value &value);

    void set_from_serialized(
        avro_value_t *value,
        const core::types::Bytes &bytes);

    std::optional<std::int32_t> get_int(
        const avro_value_t &avro_value);

    std::optional<std::int64_t> get_long(
        const avro_value_t &avro_value);

    std::optional<float> get_float(
        const avro_value_t &avro_value);

    std::optional<double> get_double(
        const avro_value_t &avro_value);

    std::optional<bool> get_boolean(
        const avro_value_t &avro_value);

    std::optional<int> get_enum_value(
        const avro_value_t &avro_value);

    std::optional<std::string> get_enum_symbol(
        const avro_value_t &avro_value);

    std::optional<std::string> get_string(
        const avro_value_t &avro_value);

    std::optional<core::types::ByteVector> get_bytes(
        const avro_value_t &avro_value);

    std::optional<core::types::ByteVector> get_fixed(
        const avro_value_t &avro_value);

    core::types::Value get_value(
        const avro_value_t &avro_value,
        bool enums_as_strings);

    core::types::KeyValueMapPtr get_map(
        const avro_value_t &avro_value,
        bool enums_as_strings);

    core::types::ValueListPtr get_array(
        const avro_value_t &avro_value,
        bool enums_as_strings);

    core::types::Value get_union(
        const avro_value_t &avro_value,
        bool enums_as_strings);

    std::set<avro_type_t> union_types(
        const avro_schema_t &schema);

    template <typename EnumType>
    EnumType get_enum(
        const avro_value_t &avro_value)
    {
        int enumvalue = 0;
        avro_value_get_enum(&avro_value, &enumvalue);
        return static_cast<EnumType>(enumvalue);
    }
}  // namespace avro
