/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.c++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-compoundvalue.h++"
#include "parsers/json/writer.h++"

namespace core::avro
{

    CompoundValue::CompoundValue(avro_schema_t schema)
        : BaseValue(),
          schema(schema),
          iface(This::checkstatus(avro_generic_class_from_schema(schema)))
    {
        This::checkstatus(avro_generic_value_new(this->iface, &this->value));
    }

    CompoundValue::CompoundValue(const std::string &json_schema)
        : BaseValue(),
          schema(This::schema_from_json(json_schema)),
          iface(This::checkstatus(avro_generic_class_from_schema(schema)))
    {
        This::checkstatus(avro_generic_value_new(this->iface, &this->value));
    }

    CompoundValue::~CompoundValue()
    {
        avro_value_iface_decref(this->iface);
        avro_schema_decref(schema);
    }

    void CompoundValue::assign(const types::Value &value)
    {
    }

    avro_schema_t CompoundValue::schema_from_json(
        const std::string &json)
    {
        avro_schema_t schema;
        This::checkstatus(avro_schema_from_json_length(
            json.data(),
            json.size(),
            &schema));
        return schema;
    }

    avro_schema_t CompoundValue::schema_from_value(
        const types::Value &value)
    {
        return This::schema_from_json(json::writer.encoded(value));
    }


    avro_schema_t CompoundValue::timestamp_schema()
    {
        return This::schema_from_value(This::timestamp_schema_as_kvmap());
    }

    avro_schema_t CompoundValue::duration_schema()
    {
        return This::schema_from_value(This::duration_schema_as_kvmap());
    }

    avro_schema_t CompoundValue::variant_value_schema()
    {
        return This::schema_from_value(This::variant_value_schema_as_kvmap());
    }

    avro_schema_t CompoundValue::variant_list_schema()
    {
        return This::schema_from_value(This::variant_list_schema_as_kvmap());
    }

    avro_schema_t CompoundValue::variant_map_schema()
    {
        return This::schema_from_value(This::variant_map_schema_as_kvmap());
    }

    types::KeyValueMap CompoundValue::timestamp_schema_as_kvmap()
    {
        return This::record_schema_as_kvmap(
            TypeName_Timestamp,
            {
                This::field_schema_as_kvmap(
                    SchemaField_TimeSeconds,
                    TypeName_Long),

                This::field_schema_as_kvmap(
                    SchemaField_TimeNanos,
                    TypeName_Int),
            });
    }

    types::KeyValueMap CompoundValue::duration_schema_as_kvmap()
    {
        return This::record_schema_as_kvmap(
            TypeName_Duration,
            {
                This::field_schema_as_kvmap(
                    SchemaField_TimeSeconds,
                    TypeName_Long),

                This::field_schema_as_kvmap(
                    SchemaField_TimeNanos,
                    TypeName_Int),
            });
    }

    types::KeyValueMap CompoundValue::variant_value_schema_as_kvmap()
    {
        return This::record_schema_as_kvmap(
            TypeName_Variant,
            {
                This::field_schema_as_kvmap(
                    "value",
                    types::ValueList({
                        TypeName_Null,
                        TypeName_Boolean,
                        TypeName_Long,
                        TypeName_Double,
                        TypeName_String,
                        TypeName_Bytes,
                        This::timestamp_schema_as_kvmap(),
                        This::duration_schema_as_kvmap(),
                        This::variant_map_schema_as_kvmap(),
                        This::variant_list_schema_as_kvmap(),
                    })),
            });
    }

    types::KeyValueMap CompoundValue::variant_map_schema_as_kvmap()
    {
        return This::map_schema_as_kvmap(TypeName_Variant);
    }

    types::KeyValueMap CompoundValue::variant_list_schema_as_kvmap()
    {
        return This::array_schema_as_kvmap(TypeName_Variant);
    }

    types::KeyValueMap CompoundValue::record_schema_as_kvmap(
        const std::string &name,
        const types::ValueList &fields)
    {
        return {
            {SchemaField_Type, TypeName_Record},
            {SchemaField_Name, name},
            {SchemaField_RecordFields, fields},
        };
    }

    types::KeyValueMap CompoundValue::field_schema_as_kvmap(
        const std::string &name,
        const types::Value &type)
    {
        return {
            {SchemaField_Type, type},
            {SchemaField_Name, name},
        };
    }

    types::KeyValueMap CompoundValue::map_schema_as_kvmap(
        const std::string &valuetype)
    {
        return {
            {SchemaField_Type, TypeName_Map},
            {SchemaField_MapValues, valuetype},
        };
    }

    types::KeyValueMap CompoundValue::array_schema_as_kvmap(
        const std::string &itemtype)
    {
        return {
            {SchemaField_Type, TypeName_Array},
            {SchemaField_ArrayItems, itemtype},
        };
    }

} // namespace core::avro
