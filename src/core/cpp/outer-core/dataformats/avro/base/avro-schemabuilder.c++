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

namespace core::avro
{
    //--------------------------------------------------------------------------
    // SchemaWrapper

    SchemaWrapper::SchemaWrapper(const types::ValueBase &value)
        : Value(value)
    {
    }

    SchemaWrapper::SchemaWrapper(types::KeyValueMap &&kvmap)
        : Value(kvmap)
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
        return json::writer.encoded(*this);
    }

    //--------------------------------------------------------------------------
    // SchemaBuilder

    SchemaBuilder::SchemaBuilder(types::KeyValueMap &&spec)
        : SchemaWrapper(std::move(spec))

    {
    }

    void SchemaBuilder::set(const std::string &key,
                            const types::Value &value)
    {
        this->get_kvmap()->insert_or_assign(key, value);
    }

    //--------------------------------------------------------------------------
    // RecordSchema

    RecordSchema::RecordSchema(const std::string &name,
                               const types::ValueList &fields)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Record},
              {SchemaField_Name, name},
              {SchemaField_RecordFields, fields},
          })
    {
    }

    //--------------------------------------------------------------------------
    // RecordField

    RecordField::RecordField(const types::Value &type,
                             const std::string &name)
        : types::KeyValueMap({
              {SchemaField_Type, type},
              {SchemaField_Name, name},
          })
    {
    }

    //--------------------------------------------------------------------------
    // MapSchema

    MapSchema::MapSchema(const types::Value &valuetype)
        : SchemaBuilder({
              {SchemaField_Type, TypeName_Map},
              {SchemaField_MapValues, valuetype},
          })
    {
    }

    //--------------------------------------------------------------------------
    // ArrayShema

    ArraySchema::ArraySchema(const types::Value &itemtype)
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
              {SchemaField_EnumSymbols, types::ValueList(symbols.begin(), symbols.end())},
          })
    {
        if (default_symbol)
        {
            this->set(SchemaField_Default, default_symbol.value());
        }
    }

    //--------------------------------------------------------------------------
    // DurationSchema

    DurationSchema::DurationSchema(const std::string &name)
        : RecordSchema(name,
                       {
                           RecordField(TypeName_Long,
                                       SchemaField_TimeSeconds),
                           RecordField(types::ValueList({TypeName_Null, TypeName_Int}),
                                       SchemaField_TimeNanos),
                       })
    {
    }

    //--------------------------------------------------------------------------
    // TimestampSchema

    TimestampSchema::TimestampSchema()
        : DurationSchema(TypeName_Timestamp)
    {
    }

    //--------------------------------------------------------------------------
    // VariantSchema

    VariantSchema::VariantSchema()
        : RecordSchema(
              TypeName_Variant,
              {
                  RecordField(
                      types::ValueList({
                          TypeName_Null,
                          TypeName_Boolean,
                          TypeName_Long,
                          TypeName_Double,
                          TypeName_String,
                          TypeName_Bytes,
                          TimestampSchema(),
                          DurationSchema(),
                          VariantMapSchema(),
                          VariantListSchema(),
                      }),
                      "value"),
              })
    {
    }

    //--------------------------------------------------------------------------
    // VariantMapSchema

    VariantMapSchema::VariantMapSchema()
        : MapSchema(TypeName_Variant)
    {
    }

    //--------------------------------------------------------------------------
    // VariantListSchema

    VariantListSchema::VariantListSchema()
        : ArraySchema(TypeName_Variant)
    {
    }

}  // namespace core::avro
