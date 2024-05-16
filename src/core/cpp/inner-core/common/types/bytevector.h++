/// -*- c++ -*-
//==============================================================================
/// @file bytevector.h++
/// @brief Packed binary data
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "streamable.h++"

#include <cstdint>
#include <ostream>
#include <vector>
#include <iomanip>

using uint = unsigned int;

namespace cc::types
{
    using Byte = std::uint8_t;

    //==========================================================================
    /// \class ByteVector
    /// \brief Container for packed binary data

    using Bytes = std::vector<Byte>;
    class ByteVector : public Bytes,
                      public Streamable
    {
    public:
        // Inherit available constructors from base
        using Bytes::Bytes;

        ByteVector(const Bytes &b)
        {
            this->assign(b.begin(), b.end());
        }

        ByteVector(const std::string &s)
        {
            this->assign(s.begin(), s.end());
        }

        void to_stream(std::ostream &stream) const override;

        /// \return string_view of data in this array
        std::string_view stringview() const noexcept;

        /// Return a string representation of the data in this array
        /// \return A byte string
        std::string as_string() const noexcept;

        /// Create a new ByteVector instance from a string
        /// \param[in] s
        ///    String containing bytes to load
        /// \return
        ///    New ByteVector instance
        static ByteVector from_string(const std::string &s) noexcept;

        /// Unpack a specific data type stored at the byte array
        /// \return Value
        /// \exception std::out_of_range The number of bytes
        template <class T>
        inline T unpack() const
        {
            if (this->size() < sizeof(T))
            {
                throw std::out_of_range("Not enough bytes to unpack");
            }
            return *(T *)this->data();
        }

        template <class T>
        static inline ByteVector pack(const T &value)
        {
            std::uint8_t *ptr = (std::uint8_t *)&value;
            return ByteVector(ptr, ptr + sizeof(T));
        }

        /// Encode the data in this ByteVector instance using Base64
        /// \return Base64-encoded string representation of the data in this array
        std::string to_base64() const;

        /// Create a new ByteVector instance from a Base64-encoded string.
        /// \param[in] string A Base64-encoded string
        /// \return A new ByteVector instance
        /// \exception exception::InvalidArgument Invalid Base64 data encountered
        static ByteVector from_base64(const std::string &string);

        /// Encode data in this ByteVector instance as a hexacecimal string.
        /// \param[in] use uppercase digits 'A'..'F'.
        /// \return Byte array represented as a hexadecimal string
        std::string to_hex(bool uppercase = false,
                           std::size_t groupsize = 0) const;

        /// Create a new ByteVector instance from a hexadecimal string.
        /// \param[in] string A string comprised of pairs of hexadecimal digits
        /// \return A new ByteVector instance
        /// \exception exception::InvalidArgument Invalid hexadecimal digits encountered
        static ByteVector from_hex(const std::string &string);
    };
}  // namespace cc::types

// Make ByteVector hashable (for unordered_map, unordered_set, etc)
namespace std
{
    template <>
    struct hash<cc::types::ByteVector>
    {
        inline std::size_t operator()(const cc::types::ByteVector &b) const
        {
            return std::hash<std::string_view>()(b.stringview());
        }
    };
}  // namespace std

// Aliases for backwards compatiblity
using Byte = cc::types::Byte;
using ByteVector = cc::types::ByteVector;
