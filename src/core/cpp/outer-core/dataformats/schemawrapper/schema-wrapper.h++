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

    std::size_t wrapped_size(
        const core::types::ByteVector &payload);

    std::size_t unwrapped_size(
        const core::types::ByteVector &wrapped);

    core::types::ByteVector wrap(
        const core::types::ByteVector &payload,
        SchemaID schema_id,
        const core::types::Byte magic = 0x00);

    bool unwrap(
        const core::types::ByteVector &wrapped,
        SchemaID *schema_id,
        core::types::ByteVector *payload,
        const core::types::Byte magic = 0x00);

    std::optional<SchemaID> extract_schema_id(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic = 0x00);

    std::shared_ptr<core::types::ByteVector> extract_payload(
        const core::types::ByteVector &wrapped,
        const core::types::Byte magic = 0x00);
}  // namespace cc::sr
