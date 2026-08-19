/// -*- c++ -*-
//==============================================================================
/// @file schema-wrapper.c++
/// @brief Local cache of schema information
/// @author Tor Slettnes
//==============================================================================

#include "schema-wrapper.h++"
#include <string.h>     // memcpy
#include <arpa/inet.h>  // htonl

namespace cc::sr
{
    std::size_t wrapped_size(
        const core::types::ByteVector &payload)
    {
        return ENVELOPE_SIZE + payload.size();
    }

    std::size_t unwrapped_size(
        const core::types::ByteVector &wrapped)
    {
        return wrapped.size() - ENVELOPE_SIZE;
    }

    core::types::ByteVector wrap(
        const core::types::ByteVector &payload,
        SchemaID schema_id,
        core::types::Byte magic)
    {
        core::types::ByteVector wrapped(ENVELOPE_SIZE + payload.size());
        wrapped[0] = magic;
        *reinterpret_cast<std::uint32_t *>(wrapped.data() + 1) = htonl(schema_id);
        memcpy(wrapped.data() + ENVELOPE_SIZE, payload.data(), payload.size());
        return wrapped;
    }

    bool unwrap(
        const core::types::ByteVector &wrapped,
        SchemaID *schema_id,
        core::types::ByteVector *payload,
        const core::types::Byte magic)
    {
        if ((wrapped.size() >= ENVELOPE_SIZE) &&
            (wrapped.at(0) == magic))
        {
            if (schema_id)
            {
                *schema_id = ntohl(
                    *reinterpret_cast<const std::uint32_t *>(
                        wrapped.data() + 1));
            }

            if (payload)
            {
                std::size_t payload_size = unwrapped_size(wrapped);
                payload->resize(payload_size);
                memcpy(payload,
                       wrapped.data() + ENVELOPE_SIZE,
                       payload_size);
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    std::optional<SchemaID> extract_schema_id(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic)
    {
        SchemaID schema_id;
        if (unwrap(wrapped, &schema_id, nullptr, magic))
        {
            return schema_id;
        }
        else
        {
            return {};
        }
    }

    std::shared_ptr<core::types::ByteVector> extract_payload(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic)
    {
        auto payload = std::make_shared<core::types::ByteVector>();
        if (unwrap(wrapped, nullptr, payload.get(), magic))
        {
            return payload;
        }
        else
        {
            return {};
        }
    }
}  // namespace cc::sr
