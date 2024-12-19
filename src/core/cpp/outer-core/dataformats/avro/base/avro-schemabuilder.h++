/// -*- c++ -*-
//==============================================================================
/// @file avro-schemabuilder.h++
/// @brief Schema generator for compound value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-status.h++"
#include "types/value.h++"

#include <avro.h>

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
    constexpr auto SchemaField_Doc = "doc";

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

    //--------------------------------------------------------------------------
    // @class SchemaWrapper
    // @brief Represents an arbitrary Avro schema

    class SchemaWrapper : public types::Value
    {
    public:
        SchemaWrapper(const types::ValueBase &value);
        SchemaWrapper(types::KeyValueMap &&kvmap);


    public:
        // @brief
        //     Convert this wrapper into a Avro-C compatible schema.
        // @return
        //     New `avro_schema_t` reference.  Use `avro_shema-decref()` the release.
        avro_schema_t as_avro_schema() const;

        std::string as_json() const;
    };

    //--------------------------------------------------------------------------
    // @class SchemaBuilder
    // @brief Construct a composite Avro schema

    class SchemaBuilder : public SchemaWrapper
    {
    protected:
        SchemaBuilder(types::KeyValueMap &&spec);

        void set(const std::string &key, const types::Value &value);
    };

    //--------------------------------------------------------------------------
    // RecordSchema

    class RecordSchema : public SchemaBuilder
    {
    public:
        RecordSchema(const std::string &name,
                     const types::ValueList &fields);
    };

    //--------------------------------------------------------------------------
    // RecordField

    class RecordField : public types::KeyValueMap
    {
    public:
        RecordField(const types::Value &type,
                    const std::string &name);
    };

    //--------------------------------------------------------------------------
    // MapSchema

    class MapSchema : public SchemaBuilder
    {
    public:
        MapSchema(const types::Value &valuetype);
    };

    //--------------------------------------------------------------------------
    // ArrayShema

    class ArraySchema : public SchemaBuilder
    {
    public:
        ArraySchema(const types::Value &itemtype);
    };

    //--------------------------------------------------------------------------
    // EnumShema

    class EnumSchema : public SchemaBuilder
    {
    public:
        EnumSchema(const std::string &name,
                   const std::vector<std::string> &symbols,
                   const std::optional<std::string> &default_symbol = {});
    };

    //--------------------------------------------------------------------------
    //DurationSchema

    class DurationSchema : public RecordSchema
    {
    public:
        DurationSchema(const std::string &name = TypeName_Duration);
    };

    //--------------------------------------------------------------------------
    // TimestampSchema

    class TimestampSchema : public DurationSchema
    {
    public:
        TimestampSchema();
    };

    //--------------------------------------------------------------------------
    // VariantSchema

    class VariantSchema : public RecordSchema
    {
    public:
        VariantSchema();
    };

    //--------------------------------------------------------------------------
    // VariantMapSchema

    class VariantMapSchema : public MapSchema
    {
    public:
        VariantMapSchema();
    };

    //--------------------------------------------------------------------------
    // VariantListSchema

    class VariantListSchema : public ArraySchema
    {
    public:
        VariantListSchema();
    };

} // namespace core::avro
