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

    SchemaWrapper::SchemaWrapper(const core::types::KeyValueMap &kvmap)
        : Value(core::types::Value(kvmap))
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

    //--------------------------------------------------------------------------
    // SchemaBuilder

    SchemaBuilder::SchemaBuilder(const core::types::KeyValueMap &spec)
        : SchemaWrapper(spec)
    {
    }

    void SchemaBuilder::set(const std::string &key,
                            const core::types::Value &value)
    {
        this->get_kvmap()->insert_or_assign(key, value);
    }

    //--------------------------------------------------------------------------
    // RecordSchema

    RecordSchema::RecordSchema(const std::string &name,
                               const core::types::ValueList &fields)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Record},
              {SchemaField_Name, name},
              {SchemaField_RecordFields, fields},
          })
    {
    }

    //--------------------------------------------------------------------------
    // RecordField

    RecordField::RecordField(const core::types::Value &type,
                             const std::string &name)
        : core::types::KeyValueMap({
              {SchemaField_Type, type},
              {SchemaField_Name, name},
          })
    {
    }

    //--------------------------------------------------------------------------
    // MapSchema

    MapSchema::MapSchema(const core::types::Value &valuetype)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Map},
              {SchemaField_MapValues, valuetype},
          })
    {
    }

    //--------------------------------------------------------------------------
    // ArrayShema

    ArraySchema::ArraySchema(const core::types::Value &itemtype)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Array},
              {SchemaField_ArrayItems, itemtype},
          })
    {
    }

    //--------------------------------------------------------------------------
    // EnumSchema

    EnumSchema::EnumSchema(const std::string &name,
                           const std::vector<std::string> &symbols,
                           const std::optional<std::string> &default_symbol)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Enum},
              {SchemaField_Name, name},
              {SchemaField_EnumSymbols, core::types::ValueList(symbols.begin(), symbols.end())},
          })
    {
        if (default_symbol)
        {
            this->set(SchemaField_Default, default_symbol.value());
        }
    }

    //--------------------------------------------------------------------------
    // DurationSchema

    DurationSchema::DurationSchema()
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Fixed},
              {SchemaField_Name, TypeName_Duration},
              {SchemaField_LogicalType, LogicalType_Duration},
              {SchemaField_Size, LogicalType_Duration_Size},
          })
    {
    }

    //--------------------------------------------------------------------------
    // TimestampSchema

    TimestampSchema::TimestampSchema()
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Long},
              {SchemaField_LogicalType, LogicalType_TimeStampMillis},
          })
    {
    }


    //--------------------------------------------------------------------------
    // VariantSchema

    VariantSchema::VariantSchema()
        : RecordSchema(
              TypeName_Variant,
              {
                  RecordField(VariantSchema::alternates, SchemaField_VariantValue),
              })
    {
    }


    core::types::ValueList VariantSchema::alternates = {
        TypeName_Null,                              // VT_NULL
        TypeName_String,                            // VT_STRING
        TypeName_Bytes,                             // VT_BYTES
        TypeName_Boolean,                           // VT_BOOL
        TypeName_Long,                              // VT_LONG
        TypeName_Double,                            // VT_DOUBLE
        TimestampSchema().get_kvmap(),              // VT_TIMESTAMP
        DurationSchema().get_kvmap(),               // VT_DURATION
        MapSchema(TypeName_Variant).get_kvmap(),    // VT_MAP
        ArraySchema(TypeName_Variant).get_kvmap(),  // VT_ARRAY
    };

    //--------------------------------------------------------------------------
    // VariantMapSchema

    VariantMapSchema::VariantMapSchema()
        : MapSchema(VariantSchema())
    {
    }

    //--------------------------------------------------------------------------
    // VariantListSchema

    VariantListSchema::VariantListSchema()
        : ArraySchema(VariantSchema())
    {
    }

}  // namespace avro
