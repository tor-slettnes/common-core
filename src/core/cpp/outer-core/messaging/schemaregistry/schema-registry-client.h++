/// -*- c++ -*-
//==============================================================================
/// @file schema-registry-client.h++
/// @brief Client for Confluent Schema Registry
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "rest-client.h++"
#include "types/value.h++"

namespace cc::sr
{
    using SchemaID = std::uint32_t;
    using SchemaName = std::string;
    using SchemaText = std::string;

    struct SchemaRecord
    {
        SchemaID id = 0;
        SchemaName name;
        core::types::Value definition;
    };

    using SchemaMap = core::types::ValueMap<SchemaName, SchemaRecord>;

    //--------------------------------------------------------------------------
    // SchemaRegistryClient

    class SchemaRegistryClient
        : public cc::http::RESTClient,
          public core::types::enable_create_shared<SchemaRegistryClient>
    {
        using This = SchemaRegistryClient;
        using Super = cc::http::RESTClient;

    public:
        using SchemaID = std::uint32_t;

    protected:
        SchemaRegistryClient(
            const std::string &registry_url,
            const std::string &registry_name = "",
            const std::string &messaging_flavor = "SchemaRegistry");

    public:
        std::string registry_url() const;
        void set_registry_url(const std::string &url);

        SchemaRecord fetch_by_id(
            SchemaID id) const;

        SchemaRecord fetch_by_name(
            const SchemaName &name) const;

        SchemaRecord register_schema(
            const SchemaName &name,
            const core::types::Value &schema);

        SchemaRecord register_schema(
            const SchemaName &name,
            const SchemaText &schema_text);

    private:
        core::types::Value extract_schema(
            const core::types::Value &response) const;
    };
}  // namespace cc::sr
