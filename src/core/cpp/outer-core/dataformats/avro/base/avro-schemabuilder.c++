/// -*- c++ -*-
//==============================================================================
/// @file avro-schemabuilder.c++
/// @brief Schema generator for compound value
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-schemabuilder.h++"
#include "avro-status.h++"
#include "parsers/json/writer.h++"
#include "logging/logging.h++"

namespace avro
{
    // --------------------------------------------------------------------------
    // SchemaWrapper

    SchemaWrapper::SchemaWrapper(const core::types::Value &value)
        : Value(value)
    {
    }

    avro_schema_t SchemaWrapper::as_avro_schema() const
    {
        std::string json = this->as_json();
        avro_schema_t schema;
        checkstatus(avro_schema_from_json_length(
            json.data(),
            json.size(),
            &schema));
        return schema;
    }

    std::string SchemaWrapper::as_json() const
    {
        return core::json::writer.encoded(*this);
    }

    void SchemaWrapper::set(const std::string &key,
                            const core::types::Value &value)
    {
        if (auto kvmap = this->get_kvmap())
        {
            kvmap->insert_or_assign(key, value);
        }
    }

    //--------------------------------------------------------------------------
    // BuilderContext

    core::types::Value BuilderContext::build(const std::string &name,
                                             core::types::TaggedValueList &&spec)
    {
        if (this->defined_schemas.count(name))
        {
            return name;
        }
        else
        {
            this->defined_schemas.insert(name);
            spec.emplace(spec.begin(), SchemaField_Name, name);
            return spec;
        }
    }

    //--------------------------------------------------------------------------
    // RecordSchema

    RecordSchema::RecordSchema(const ContextRef &context,
                               const std::string &name)
        : SchemaWrapper(context->build(
              name,
              {
                  {SchemaField_Type, TypeName_Record},
                  {SchemaField_RecordFields, core::types::ValueList()},
              })),
          context(context)
    {
    }

    void RecordSchema::add_field(const std::string &name,
                                 const core::types::Value &type)
    {
        if (auto fields = this->get(std::string(SchemaField_RecordFields)).get_valuelist())
        {
            fields->push_back(core::types::TaggedValueList(
                {
                    {SchemaField_Name, name},
                    {SchemaField_Type, type},
                }));
        }
    }

    //--------------------------------------------------------------------------
    // MapSchema

    MapSchema::MapSchema(const core::types::Value &valuetype)
        : SchemaWrapper(core::types::KeyValueMap({
              {SchemaField_Type, TypeName_Map},
              {SchemaField_MapValues, valuetype},
          }))
    {
    }

    //--------------------------------------------------------------------------
    // ArrayShema

    ArraySchema::ArraySchema(const core::types::Value &itemtype)
        : SchemaWrapper(core::types::KeyValueMap({
              {SchemaField_Type, TypeName_Array},
              {SchemaField_ArrayItems, itemtype},
          }))
    {
    }

    //--------------------------------------------------------------------------
    // EnumSchema

    EnumSchema::EnumSchema(const ContextRef &context,
                           const std::string &name,
                           const std::vector<std::string> &symbols,
                           const std::optional<std::string> &default_symbol)
        : SchemaWrapper(context->build(
              name,
              {
                  {SchemaField_Type, TypeName_Enum},
                  {SchemaField_EnumSymbols, core::types::ValueList(symbols.begin(), symbols.end())},
              }))
    {
        if (default_symbol)
        {
            this->set(SchemaField_Default, default_symbol.value());
        }
    }

    //--------------------------------------------------------------------------
    // DurationSchema

    CalendarTimeIntervalSchema::CalendarTimeIntervalSchema(const ContextRef &context)
        : SchemaWrapper(context->build(
              TypeName_CalendarTimeInterval,
              {
                  {SchemaField_Type, TypeName_Fixed},
                  {SchemaField_LogicalType, LogicalType_Duration},
                  {SchemaField_Size, LogicalType_Duration_Size},
              }))
    {
    }

    //--------------------------------------------------------------------------
    // TimeIntervalSchema

    TimeIntervalSchema::TimeIntervalSchema(const ContextRef &context)
        : RecordSchema(context, TypeName_TimeInterval)
    {
        this->add_field(SchemaField_TimeSeconds, TypeName_Long);
        this->add_field(SchemaField_TimeNanos, TypeName_Int);
    }

    //--------------------------------------------------------------------------
    // TimestampSchema

    TimestampSchema::TimestampSchema(const ContextRef &context)
        : RecordSchema(context, TypeName_Timestamp)
    {
        this->add_field(SchemaField_TimeSeconds, TypeName_Long);
        this->add_field(SchemaField_TimeNanos, TypeName_Int);
    }

    //--------------------------------------------------------------------------
    // VariantSchema

    VariantSchema::VariantSchema(const ContextRef &context)
        : RecordSchema(context, TypeName_Variant)
    {
        core::types::ValueList subtypes;
        subtypes.push_back(TypeName_Null);                  // VT_NULL
        subtypes.push_back(TypeName_String);                // VT_STRING
        subtypes.push_back(TypeName_Bytes);                 // VT_BYTES
        subtypes.push_back(TypeName_Boolean);               // VT_BOOL
        subtypes.push_back(TypeName_Long);                  // VT_LONG
        subtypes.push_back(TypeName_Double);                // VT_DOUBLE
        subtypes.push_back(TimeIntervalSchema(context));    // VT_INTERVAL
        subtypes.push_back(TimestampSchema(context));       // VT_TIMESTAMP
        subtypes.push_back(MapSchema(TypeName_Variant));    // VT_MAP
        subtypes.push_back(ArraySchema(TypeName_Variant));  // VT_ARRAY
        this->add_field(SchemaField_VariantValue, subtypes);
    }

    //--------------------------------------------------------------------------
    // VariantMapSchema

    VariantMapSchema::VariantMapSchema(const ContextRef &context)
        : MapSchema(VariantSchema(context))
    {
    }

    //--------------------------------------------------------------------------
    // VariantListSchema

    VariantListSchema::VariantListSchema(const ContextRef &context)
        : ArraySchema(VariantSchema(context))
    {
    }

}  // namespace avro
