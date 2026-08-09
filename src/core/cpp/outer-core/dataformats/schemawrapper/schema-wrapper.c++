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
    core::types::ByteVector SchemaWrapper::wrap(
        const core::types::ByteVector& payload,
        SchemaID schema_id,
        core::types::Byte magic) const
    {
        core::types::ByteVector wrapped(ENVELOPE_SIZE + payload.size());
        wrapped[0] = magic;
        *reinterpret_cast<std::uint32_t*>(wrapped.data() + 1) = htonl(schema_id);
        memcpy(wrapped.data() + ENVELOPE_SIZE, payload.data(), payload.size());
        return wrapped;
    }

    core::types::ByteVector SchemaWrapper::wrap(
        const UnwrappedPayload& unwrapped) const
    {
        return this->wrap(unwrapped.payload, unwrapped.id, unwrapped.magic);
    }

    std::optional<UnwrappedPayload> SchemaWrapper::unwrap(
        const core::types::ByteVector& wrapped) const
    {
        if (wrapped.size() >= ENVELOPE_SIZE)
        {
            UnwrappedPayload unwrapped;
            unwrapped.magic = wrapped[0];
            unwrapped.id = ntohl(*reinterpret_cast<const std::uint32_t*>(wrapped.data() + 1));

            std::size_t payload_size = wrapped.size() - ENVELOPE_SIZE;
            memcpy(unwrapped.payload.data(),
                   wrapped.data() + ENVELOPE_SIZE,
                   payload_size);

            return unwrapped;
        }
        else
        {
            return {};
        }
    }

    std::size_t SchemaWrapper::wrapped_size(const core::types::ByteVector& original) const
    {
        return ENVELOPE_SIZE + original.size();
    }

    std::size_t SchemaWrapper::wrapped_size(const UnwrappedPayload& unwrapped) const
    {
        return ENVELOPE_SIZE + unwrapped.payload.size();
    }

}  // namespace cc::sr
