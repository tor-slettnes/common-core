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

namespace avro
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
    constexpr auto TypeName_Fixed = "fixed";
    constexpr auto TypeName_Complex = "Complex";
    constexpr auto TypeName_EnumValue = "EnumValue";

    constexpr auto TypeName_Variant = "Variant";
    constexpr auto TypeName_CalendarTimeInterval = "CalendarTimeInterval";
    constexpr auto TypeName_TimeInterval = "TimeInterval";
    constexpr auto TypeName_Timestamp = "Timestamp";

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
    constexpr auto SchemaField_Size = "size";
    constexpr auto SchemaField_ComplexReal = "real";
    constexpr auto SchemaField_ComplexImaginary = "imag";
    constexpr auto SchemaField_VariantValue = "variant";
    constexpr auto SchemaField_EnumIndex = "enumeration";
    constexpr auto SchemaField_EnumValue = "value";

    constexpr auto LogicalType_TimeStampMillis = "timestamp-millis";
    constexpr auto LogicalType_TimeOfDayMillis = "time-millis";
    constexpr auto LogicalType_Duration = "duration";
    constexpr auto LogicalType_Duration_Size = 12;

    //--------------------------------------------------------------------------
    // @class SchemaWrapper
    // @brief Represents an arbitrary Avro schema

    class SchemaWrapper : public core::types::Value
    {
    public:
        SchemaWrapper(const core::types::Value &value);
        ~SchemaWrapper();

    public:
        // @brief
        //     Convert this wrapper into a Avro-C compatible schema.
        // @return
        //     New `avro_schema_t` reference.  Use `avro_shema-decref()` to release.
        avro_schema_t as_avro_schema();

        std::string as_json() const;

    protected:
        void set(const std::string &key, const core::types::Value &value);

    private:
        avro_schema_t avro_schema;
    };

    //--------------------------------------------------------------------------
    // @class BuilderContext

    class BuilderContext
    {
    public:
        core::types::Value build(const std::string &name,
                                 core::types::TaggedValueList &&spec);

    public:
        std::unordered_set<std::string> defined_schemas;
    };

    using ContextRef = std::shared_ptr<BuilderContext>;

    //--------------------------------------------------------------------------
    // RecordSchema

    class RecordSchema : public SchemaWrapper
    {
    public:
        RecordSchema(const ContextRef &context,
                     const std::string &name);

        void add_field(const std::string &name,
                       const core::types::Value &type,
                       const std::optional<std::string> &doc = {});

    protected:
        ContextRef context;
    };

    //--------------------------------------------------------------------------
    // MapSchema

    class MapSchema : public SchemaWrapper
    {
    public:
        MapSchema(const core::types::Value &valuetype);
    };

    //--------------------------------------------------------------------------
    // ArrayShema

    class ArraySchema : public SchemaWrapper
    {
    public:
        ArraySchema(const core::types::Value &itemtype);
    };

    //--------------------------------------------------------------------------
    // EnumShema - Sequential enumeration

    class EnumSchema : public SchemaWrapper
    {
    public:
        EnumSchema(const ContextRef &context,
                   const std::string &name,
                   const std::vector<std::string> &symbols,
                   const std::optional<std::string> &default_symbol = {},
                   const std::optional<std::string> &doc = {});
    };

    //--------------------------------------------------------------------------
    // EnumValueShema - Enumeration of arbitrary integer values

    class EnumValueSchema : public RecordSchema
    {
    public:
        EnumValueSchema(const ContextRef &context,
                        const std::string &name,
                        const std::vector<std::string> &symbols,
                        const std::optional<std::string> &default_symbol = {},
                        const std::optional<std::string> &doc = {});
    };

    //--------------------------------------------------------------------------
    /// @class CalendarTimeIntervalSchema
    /// @brief
    ///   Wrapper around Avro's `duration` logical schema, representing a
    ///   date/time interval in terms of (milli)seconds, days, and months.

    class CalendarTimeIntervalSchema : public SchemaWrapper
    {
    public:
        CalendarTimeIntervalSchema(const ContextRef &context);
    };

    //--------------------------------------------------------------------------
    /// @class TimeIntervalSchema
    /// @brief
    ///    Represents a plain time interval (i.e. elapsed time).
    /// @note
    ///    We avoid the name `Duration` since it is easily confused with
    ///    Avro's `duration` logical type, which in turn is best suited for
    ///    representing calendar time intervals (years/months, days, seconds).

    // class TimeIntervalSchema : public RecordSchema
    // {
    // public:
    //     TimeIntervalSchema(const ContextRef &context);
    // };

    class TimeIntervalSchema : public SchemaWrapper
    {
    public:
        TimeIntervalSchema(const ContextRef &context);
    };


    //--------------------------------------------------------------------------
    /// @class TimestampSchema
    /// @brief
    ///     Represents an absolute timestamp as relative time since
    ///     UNIX epoch.

    class TimestampSchema : public SchemaWrapper
    {
    public:
        TimestampSchema(const ContextRef &context);
    };

    //--------------------------------------------------------------------------
    // VariantSchema

    class VariantSchema : public RecordSchema
    {
    public:
        enum Type
        {
            VT_NULL,       // 0
            VT_STRING,     // 1
            VT_BYTES,      // 2
            VT_BOOL,       // 3
            VT_LONG,       // 4
            VT_DOUBLE,     // 5
            // VT_INTERVAL,   // 6
            // VT_TIMESTAMP,  // 7
            // VT_MAP,        // 8
            // VT_ARRAY       // 9
        };

        VariantSchema(const ContextRef &context);

    private:
        static core::types::ValueList alternates;
    };

    //--------------------------------------------------------------------------
    // VariantMapSchema

    class VariantMapSchema : public MapSchema
    {
    public:
        VariantMapSchema(const ContextRef &context);
    };

    //--------------------------------------------------------------------------
    // VariantListSchema

    class VariantListSchema : public ArraySchema
    {
    public:
        VariantListSchema(const ContextRef &context);
    };

}  // namespace avro
