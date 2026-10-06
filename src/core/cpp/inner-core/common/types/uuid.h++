/// -*- c++ -*-
//==============================================================================
/// @file uuid.h++
/// @brief Universally Unique Identifier
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "bytevector.h++"
#include <array>

namespace cc::core::types
{
    constexpr size_t UUID_SIZE = 16;

    class UUID: public std::array<Byte, UUID_SIZE>,
                public Streamable
    {
        using This = UUID;
        using Super = std::array<Byte, UUID_SIZE>;

    public:
        UUID() {}
        UUID(const UUID &other);
        UUID(const std::string &string);
        UUID(const ByteVector &bytes);

        void populate_from_string(const std::string &string);
        void populate_from_bytevector(const ByteVector &bytes);

    protected:
        void populate_from_raw_bytes(const Byte *bytes);

    public:
        ByteVector to_bytevector() const noexcept;
        void to_stream(std::ostream &stream) const override;
    };
}
