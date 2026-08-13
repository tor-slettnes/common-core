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
    using SchemaSubject = std::string;
    using SchemaName = std::string;
    using SchemaText = std::string;

    struct SchemaRecord
    {
        SchemaID id = 0;
        SchemaSubject subject;
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

        SchemaRecord fetch_by_subject(
            const SchemaSubject &subject) const;

        SchemaRecord register_schema(
            const SchemaSubject &subject,
            const core::types::Value &schema);

        SchemaRecord register_schema(
            const SchemaSubject &subject,
            const SchemaText &schema_text);

    private:
        std::string extract_name(
            const core::types::Value &response) const;

        core::types::Value extract_schema(
            const core::types::Value &response) const;
    };
}  // namespace cc::sr
