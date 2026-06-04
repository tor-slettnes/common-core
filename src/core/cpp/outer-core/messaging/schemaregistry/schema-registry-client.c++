/// -*- c++ -*-
//==============================================================================
/// @file schema-registry-client.c++
/// @brief Client for Confluent Schema Registry
/// @author Tor Slettnes
//==============================================================================

#include "schema-registry-client.h++"
#include "string/expand.h++"
#include "parsers/json/reader.h++"
#include "parsers/json/writer.h++"

namespace cc::sr
{
    SchemaRegistryClient::SchemaRegistryClient(
        const std::string &registry_url,
        const std::string &registry_name,
        const std::string &messaging_flavor)
        : Super(registry_url,
                registry_name,
                "SchemaRegistry",
                "application/vnd.schemaregistry.v1+json")
    {
    }

    std::string SchemaRegistryClient::registry_url() const
    {
        return this->real_url({});
    }

    void SchemaRegistryClient::set_registry_url(const std::string &url)
    {
        (*this->settings())[this->channel_name()]["url"] = url;
        this->settings()->save();
    }


    SchemaRecord SchemaRegistryClient::fetch_by_id(
        SchemaID id) const
    {
        std::string rel_path = core::str::expand(
            this->setting("fetch_by_id").as_string(),
            {
                {"id", core::str::convert_from(id)},
            });

        core::types::Value response = this->get_json(rel_path);
        core::types::Value schema = this->extract_schema(response);

        return {
            .id = id,
            .name = schema.get("name").to_string(),
            .definition = schema,
        };
    }

    SchemaRecord SchemaRegistryClient::fetch_by_name(
        const SchemaName &name) const
    {
        std::string rel_path = core::str::expand(
            this->setting("fetch_by_name").as_string(),
            {
                {"name", name},
            });

        core::types::Value response = this->get_json(rel_path);
        return {
            .id = response.get("id").as_uint32(),
            .name = response.get("subject").to_string(),
            .definition = this->extract_schema(response),
        };
    }

    SchemaRecord SchemaRegistryClient::register_schema(
        const SchemaName &name,
        const core::types::Value &schema)
    {
        return this->register_schema(name, core::json::writer.encoded(schema));
    }


    SchemaRecord SchemaRegistryClient::register_schema(
        const SchemaName &name,
        const SchemaText &schema_text)
    {
        std::string rel_path = core::str::expand(
            this->setting("register").as_string(),
            {
                {"name", name},
            });

        core::types::KeyValueMap request = {
            {"schema", schema_text},
        };

        core::types::Value response = this->post_json(rel_path, request);
        return {
            .id = response.get("id").as_uint32(),
            .name = name,
            .definition = this->extract_schema(response),
        };
    }

    core::types::Value SchemaRegistryClient::extract_schema(
        const core::types::Value &response) const
    {
        core::types::Value schema = response.get("schema");
        if (schema.is_string())
        {
            return core::json::reader.decoded(schema.as_string());
        }
        else
        {
            return schema;
        }
    }

}  // namespace cc::sr
