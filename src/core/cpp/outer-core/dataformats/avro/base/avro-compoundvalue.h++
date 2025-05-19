/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.h++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-basevalue.h++"
#include "avro-schemabuilder.h++"
#include "types/value.h++"

namespace avro
{
    class CompoundValue : public BaseValue
    {
        using This = CompoundValue;
        using Super = BaseValue;

    public:
        CompoundValue(avro_schema_t schema);
        CompoundValue(const std::string &json_schema);
        CompoundValue(SchemaWrapper &&wrapper);

    public:
        ~CompoundValue();

    private:
        static avro_schema_t schema_from_json(const std::string &json_schema);

    public:
        static avro_value_t get_by_index(
            avro_value_t *value,
            int index,
            const std::optional<std::string> &expected_name = {});

        static avro_value_t get_by_name(
            avro_value_t *value,
            const std::string &name,
            const std::optional<std::size_t> &expected_index = {});

        static void set_enum_value(avro_value_t *value,
                                   std::uint64_t index,
                                   std::int64_t number);

        static void set_datetime_interval(avro_value_t *value,
                                          const core::dt::DateTimeInterval &interval);
        static void set_time_interval(avro_value_t *value,
                                      const core::dt::Duration &dur);
        static void set_timestamp(avro_value_t *value,
                                  const core::dt::TimePoint &tp);
        static void set_variant(avro_value_t *value,
                                const core::types::Value &variant);
        static void set_variant_list(avro_value_t *value,
                                     const core::types::ValueList &list);
        static void set_variant_map(avro_value_t *value,
                                    const core::types::KeyValueMap &kvmap);

        static void set_branch(avro_value_t *value,
                               VariantSchema::Type type,
                               avro_value_t *branch);

    protected:
        avro_schema_t schema;
        avro_value_iface_t *iface;
    };
}  // namespace avro
