/// -*- c++ -*-
//==============================================================================
/// @file uuid.c++
/// @brief Universally Unique Identifier
/// @author Tor Slettnes
//==============================================================================

#include "uuid.h++"
#include "status/exceptions.h++"
#include <unordered_set>
#include <iomanip>

namespace cc::core::types
{
    UUID::UUID(const UUID &other)
    {
        this->populate_from_raw_bytes(other.data());
    }

    UUID::UUID(const std::string &string)
    {
        this->populate_from_string(string);
    }

    UUID::UUID(const ByteVector &bytes)
    {
        this->populate_from_bytevector(bytes);
    }

    void UUID::populate_from_string(const std::string &string)
    {
        this->populate_from_bytevector(ByteVector::from_hex(string, {'-'}));
    }

    void UUID::populate_from_bytevector(const ByteVector &bytes)
    {
        if (bytes.size() == UUID_SIZE)
        {
            this->populate_from_raw_bytes(bytes.data());
        }
        else
        {
            throwf(
                std::invalid_argument,
                "Cannot construct UUID from %d bytes; need exactly %d",
                bytes.size(),
                UUID_SIZE);
        }
    }

    void UUID::populate_from_raw_bytes(const Byte *bytes)
    {
        for (std::size_t i = 0; i < UUID_SIZE; i++)
        {
            this->at(i) = bytes[i];
        }
    }

    ByteVector UUID::to_bytevector() const noexcept
    {
        return ByteVector(this->begin(), this->end());
    }

    void UUID::to_stream(std::ostream &stream) const
    {
        static std::unordered_set<std::size_t> dash_positions{4, 6, 8, 10};
        std::ios::fmtflags original_flags{stream.flags()};
        stream << std::hex
               << std::noshowbase
               << std::setfill('0');

        for (std::size_t i = 0; i < UUID_SIZE; i++)
        {
            if (dash_positions.count(i))
            {
                stream << "-";
            }
            stream << std::setw(2) << +this->at(i);
        }
        stream.flags(original_flags);
    }
}  // namespace cc::core::types
