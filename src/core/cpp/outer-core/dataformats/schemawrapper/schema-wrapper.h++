/// -*- c++ -*-
//==============================================================================
/// @file schema-wrapper.h++
/// @brief Local cache of schema information
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/create-shared.h++"

#include <optional>

namespace cc::sr
{
    using SchemaID = std::uint32_t;
    using SchemaName = std::string;
    constexpr auto ENVELOPE_SIZE = sizeof(core::types::Byte) + sizeof(SchemaID);

    struct UnwrappedPayload
    {
        core::types::ByteVector payload;
        SchemaID id = 0;
        core::types::Byte magic = 0x00;
    };

    class SchemaWrapper
        : public core::types::enable_create_shared<SchemaWrapper>
    {
    public:
        core::types::ByteVector wrap(
            const core::types::ByteVector& payload,
            SchemaID schema_id,
            core::types::Byte magic = 0x00) const;

        core::types::ByteVector wrap(
            const UnwrappedPayload& unwrapped) const;

        std::optional<UnwrappedPayload> unwrap(
            const core::types::ByteVector& wrapped) const;

        std::size_t wrapped_size(const core::types::ByteVector& original) const;
        std::size_t wrapped_size(const UnwrappedPayload& unwrapped) const;
    };
}  // namespace cc::sr
