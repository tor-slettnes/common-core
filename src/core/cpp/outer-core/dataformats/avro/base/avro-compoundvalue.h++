/// -*- c++ -*-
//==============================================================================
/// @file avro-compoundvalue.h++
/// @brief Wrapper for compound Avro value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-basevalue.h++"
#include "types/value.h++"

namespace core::avro
{
    constexpr auto TypeName_String = "string";
    constexpr auto TypeName_Bytes = "bytes";
    constexpr auto TypeName_Int = "int";
    constexpr auto TypeName_Long = "long";
    constexpr auto TypeName_Float = "float";
    constexpr auto TypeName_Double = "double";
    constexpr auto TypeName_Boolean = "boolean";
    constexpr auto TypeName_Null = "null";
    constexpr auto TypeName_Record = "record";
    constexpr auto TypeName_Enum = "enum";
    constexpr auto TypeName_Map = "map";
    constexpr auto TypeName_Array = "array";

    constexpr auto TypeName_Variant = "Variant";
    constexpr auto TypeName_Timestamp = "Timestamp";
    constexpr auto TypeName_Duration = "Duration";

    constexpr auto SchemaField_Type = "type";
    constexpr auto SchemaField_Name = "name";
    constexpr auto SchemaField_NameSpace = "namespace";
    constexpr auto SchemaField_Aliases = "aliases";
    constexpr auto SchemaField_Default = "default";

    constexpr auto SchemaField_LogicalType = "logicalType";
    constexpr auto SchemaField_RecordFields = "fields";
    constexpr auto SchemaField_RecordOrder = "order";
    constexpr auto SchemaField_ArrayItems = "items";
    constexpr auto SchemaField_MapValues = "values";
    constexpr auto SchemaField_EnumSymbols = "symbols";
    constexpr auto SchemaField_TimeSeconds = "seconds";
    constexpr auto SchemaField_TimeNanos = "nanoseconds";

    constexpr auto LogicalType_TimeStampMillis = "timestamp-millis";
    constexpr auto LogicalType_TimeOfDayMillis = "time-millis";

    class CompoundValue : public BaseValue
    {
        using This = CompoundValue;
        using Super = BaseValue;

    protected:
        CompoundValue(avro_schema_t schema);
        CompoundValue(const std::string &json_schema);

    public:
        ~CompoundValue();

        void assign(const types::Value &value);

    public:
        static avro_schema_t schema_from_json(
            const std::string &json);

        static avro_schema_t schema_from_value(
            const types::Value &value);

        static avro_schema_t timestamp_schema();
        static avro_schema_t duration_schema();
        static avro_schema_t variant_value_schema();
        static avro_schema_t variant_list_schema();
        static avro_schema_t variant_map_schema();

    protected:
        static types::KeyValueMap timestamp_schema_as_kvmap();
        static types::KeyValueMap duration_schema_as_kvmap();
        static types::KeyValueMap variant_value_schema_as_kvmap();
        static types::KeyValueMap variant_map_schema_as_kvmap();
        static types::KeyValueMap variant_list_schema_as_kvmap();

        static types::KeyValueMap record_schema_as_kvmap(
            const std::string &name,
            const types::ValueList &fields);

        static types::KeyValueMap field_schema_as_kvmap(
            const std::string &name,
            const types::Value &type);

        static types::KeyValueMap map_schema_as_kvmap(
            const std::string &valuetype);

        static types::KeyValueMap array_schema_as_kvmap(
            const std::string &itemtype);

    protected:
        avro_schema_t schema;
        avro_value_iface_t *iface;
    };
} // namespace core::avro
