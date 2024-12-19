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

namespace core::avro
{
    class CompoundValue : public BaseValue
    {
        using This = CompoundValue;
        using Super = BaseValue;

    protected:
        CompoundValue(avro_schema_t schema);
        CompoundValue(const std::string &json_schema);
        CompoundValue(const SchemaBuilder &builder);

    public:
        ~CompoundValue();

        void assign(const types::Value &value);

    private:
        static avro_schema_t schema_from_json(const std::string &json_schema);

    protected:
        avro_schema_t schema;
        avro_value_iface_t *iface;
    };
} // namespace core::avro
