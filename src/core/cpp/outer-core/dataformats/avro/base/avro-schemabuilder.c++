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
        std::cerr << "SchemaWrapper(" << kvmap
                  << ") -> "
                  << *this
                  << std::endl;
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
              {SchemaField_Type, TypeName_Long},
              {SchemaField_LogicalType, LogicalType_TimeOfDayMillis},
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
    // ComplexSchema

    ComplexSchema::ComplexSchema()
        : RecordSchema(
              TypeName_Complex,
              {
                  RecordField(TypeName_Double, SchemaField_ComplexReal),
                  RecordField(TypeName_Double, SchemaField_ComplexImaginary),
              })
    {
    }

    //--------------------------------------------------------------------------
    // VariantSchema

    VariantSchema::VariantSchema()
        : RecordSchema(
              TypeName_Variant,
              {
                  RecordField(
                      core::types::ValueList({
                          TypeName_Null,
                          TypeName_Boolean,
                          TypeName_Long,
                          TypeName_Double,
                          TypeName_String,
                          TypeName_Bytes,
                          TimestampSchema().get_kvmap(),
                          DurationSchema().get_kvmap(),
                          MapSchema(TypeName_Variant).get_kvmap(),
                          ArraySchema(TypeName_Variant).get_kvmap(),
                      }),
                      SchemaField_VariantValue),
              })
    {
    }

    // VariantSchema::TypeMap VariantSchema::type_map = {
    //     {core::types::ValueType::NONE, TypeName_Null},
    //     {core::types::ValueType::BYTEVECTOR, TypeName_Bytes},
    //     {core::types::ValueType::STRING, TypeName_String},
    //     {core::types::ValueType::CHAR, TypeName_String},
    //     {core::types::ValueType::BOOL, TypeName_Boolean},
    //     {core::types::ValueType::UINT, TypeName_Long},
    //     {core::types::ValueType::SINT, TypeName_Long},
    //     {core::types::ValueType::REAL, TypeName_Double},
    //     {core::types::ValueType::COMPLEX, ComplexSchema().get_kvmap()},
    //     {core::types::ValueType::TIMEPOINT, TimestampSchema().get_kvmap()},
    //     {core::types::ValueType::DURATION, DurationSchema().get_kvmap()},
    //     {core::types::ValueType::KVMAP, MapSchema().get_kvmap()},
    //     {core::types::ValueType::VALUELIST, ArraySchema().get_kvmap()},
    // };

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
