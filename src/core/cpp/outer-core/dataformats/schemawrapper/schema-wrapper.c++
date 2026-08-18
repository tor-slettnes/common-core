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
    std::size_t SchemaWrapper::wrapped_size(
        const core::types::ByteVector &payload) const
    {
        return ENVELOPE_SIZE + payload.size();
    }

    std::size_t SchemaWrapper::unwrapped_size(
        const core::types::ByteVector &wrapped) const
    {
        return wrapped.size() - ENVELOPE_SIZE;
    }

    core::types::ByteVector SchemaWrapper::wrap(
        const core::types::ByteVector &payload,
        SchemaID schema_id,
        core::types::Byte magic) const
    {
        core::types::ByteVector wrapped(ENVELOPE_SIZE + payload.size());
        wrapped[0] = magic;
        *reinterpret_cast<std::uint32_t *>(wrapped.data() + 1) = htonl(schema_id);
        memcpy(wrapped.data() + ENVELOPE_SIZE, payload.data(), payload.size());
        return wrapped;
    }

    bool SchemaWrapper::unwrap(
        const core::types::ByteVector &wrapped,
        SchemaID *schema_id,
        core::types::ByteVector *payload,
        const core::types::Byte magic) const
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
                std::size_t payload_size = this->unwrapped_size(wrapped);
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

    std::optional<SchemaID> SchemaWrapper::extract_schema_id(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic) const
    {
        SchemaID schema_id;
        if (this->unwrap(wrapped, &schema_id, nullptr, magic))
        {
            return schema_id;
        }
        else
        {
            return {};
        }
    }

    std::shared_ptr<core::types::ByteVector> SchemaWrapper::extract_payload(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic) const
    {
        auto payload = std::make_shared<core::types::ByteVector>();
        if (this->unwrap(wrapped, nullptr, payload.get(), magic))
        {
            return payload;
        }
        else
        {
            return {};
        }
    }
}  // namespace cc::sr
