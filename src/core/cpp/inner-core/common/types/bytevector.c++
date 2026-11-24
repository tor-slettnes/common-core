/// -*- c++ -*-
//==============================================================================
/// @file bytevector.c++
/// @brief Packed binary data
/// @author Tor Slettnes
//==============================================================================

#include "bytevector.h++"
#include "status/exception.h++"

#include <stdexcept>
#include <cstddef>
#include <cctype>

namespace cc::core::types
{
    static const char pad_char = '=';

    ByteVector::ByteVector(const Bytes &b)
    {
        this->assign(b.begin(), b.end());
    }

    ByteVector::ByteVector(const std::string &s)
    {
        this->assign(s.begin(), s.end());
    }

    ByteVector::ByteVector(const std::string_view &s)
    {
        this->assign(s.begin(), s.end());
    }

    void ByteVector::to_stream(std::ostream &stream) const
    {
        const std::string &digits = This::hex_digits.at(0);

        for (Byte byte : *this)
        {
            if (byte == '\\')
            {
                stream << "\\\\";
            }
            else if ((byte >= 0x20) && (byte < 0x7F))
            {
                stream << byte;
            }
            else
            {
                stream << "\\x"
                       << digits.at(byte >> 4)
                       << digits.at(byte & 0xF);
            }
        }
    }

    std::string_view ByteVector::stringview() const noexcept
    {
        return std::string_view(reinterpret_cast<const char *>(this->data()),
                                this->size());
    }

    std::string ByteVector::as_string() const noexcept
    {
        return std::string(reinterpret_cast<const char *>(this->data()),
                           this->size());
    }

    ByteVector ByteVector::from_pointer(
        const void *ptr,
        std::size_t size) noexcept
    {
        return {
            static_cast<const Byte *>(ptr),
            static_cast<const Byte *>(ptr) + size,
        };
    }

    ByteVector ByteVector::from_string(
        const std::string &s) noexcept
    {
        return ByteVector(s.begin(), s.end());
    }

    std::shared_ptr<ByteVector> ByteVector::shared_from_string(
        const std::string &s) noexcept
    {
        return std::make_shared<ByteVector>(s.begin(), s.end());
    }

    std::string ByteVector::to_base64() const
    {
        const std::size_t size = this->size();
        std::string encoded;
        encoded.reserve(((size / 3) + (size % 3 > 0)) * 4);

        std::uint32_t temp;
        auto it = this->begin();

        for (std::size_t i = 0; i < size / 3; ++i)
        {
            temp = (*it++) << 16;
            temp += (*it++) << 8;
            temp += (*it++);
            encoded.append(1, This::b64_symbols[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, This::b64_symbols[(temp & 0x0003F000) >> 12]);
            encoded.append(1, This::b64_symbols[(temp & 0x00000FC0) >> 6]);
            encoded.append(1, This::b64_symbols[(temp & 0x0000003F)]);
        }

        switch (size % 3)
        {
        case 1:
            temp = (*it++) << 16;
            encoded.append(1, This::b64_symbols[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, This::b64_symbols[(temp & 0x0003F000) >> 12]);
            encoded.append(2, pad_char);
            break;

        case 2:
            temp = (*it++) << 16;
            temp += (*it++) << 8;
            encoded.append(1, This::b64_symbols[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, This::b64_symbols[(temp & 0x0003F000) >> 12]);
            encoded.append(1, This::b64_symbols[(temp & 0x00000FC0) >> 6]);
            encoded.append(1, pad_char);
            break;
        }
        return encoded;
    }

    ByteVector ByteVector::from_base64(
        const std::string_view &input,
        bool ignore_whitespace)
    {
        ByteVector decoded;
        decoded.reserve((input.length() / 4) * 3);

        std::size_t sextet_group = 0;
        std::uint32_t temp = 0;
        This::init_b64_values();

        for (auto it = input.begin(); it < input.end(); it++)
        {
            if (auto value = This::b64_values.at(*it); value >= 0)
            {
                temp = (temp << 6) | value;
                if (++sextet_group == 4)
                {
                    decoded.push_back((temp >> 16) & 0x000000FF);
                    decoded.push_back((temp >> 8) & 0x000000FF);
                    decoded.push_back(temp & 0x000000FF);
                    temp = 0;
                    sextet_group = 0;
                }
            }
            else if (*it == pad_char)
            {
                switch (sextet_group)
                {
                case 2:
                    decoded.push_back((temp >> 4) & 0x000000FF);
                    return decoded;

                case 3:
                    decoded.push_back((temp >> 10) & 0x000000FF);
                    decoded.push_back((temp >> 2) & 0x000000FF);
                    return decoded;

                default:
                    throwf(std::invalid_argument,
                           "Misaligned padding in base64 input: "
                           "expected final group of 2-4 sextets; got %d",
                           sextet_group);
                }
            }
            else if (!std::isspace(*it))
            {
                throwf(
                    std::invalid_argument,
                    "Invalid base64 character at input offset %d: '%c'",
                    std::distance(input.begin(), it),
                    *it);
            }
        }

        if (sextet_group)
        {
            throwf(std::invalid_argument,
                   "Invalid base64 string length: "
                   "must contain a multiple of 4 characters; got %s",
                   decoded.size() * 4 / 3 + sextet_group);
        }

        decoded.shrink_to_fit();
        return decoded;
    }

    std::optional<ByteVector> ByteVector::try_from_base64(const std::string_view &input)
    {
        try
        {
            return ByteVector::from_base64(input);
        }
        catch (const std::invalid_argument &)
        {
            return {};
        }
    }

    std::string ByteVector::to_hex(
        bool uppercase,
        std::size_t groupsize,
        std::size_t maxsize) const
    {
        const std::string &xdigits = This::hex_digits.at(uppercase);

        std::string ellipsis;
        std::size_t size = this->size();
        if ((maxsize > 0) && (maxsize < size))
        {
            size = maxsize;
            ellipsis = "...";
        }

        uint ndigits = size * 2;
        uint nspaces = groupsize ? (size / groupsize) : 0;

        std::string encoded;
        encoded.reserve(ndigits + nspaces + ellipsis.size());

        for (std::size_t counter = 0; counter < size; counter++)
        {
            if (groupsize && counter && (counter % groupsize == 0))
            {
                encoded.push_back(' ');
            }

            std::uint8_t byte = this->at(counter);
            encoded.push_back(xdigits.at((byte >> 4) & 0xF));
            encoded.push_back(xdigits.at(byte & 0xF));
        }

        encoded += ellipsis;
        return encoded;
    }

    ByteVector ByteVector::from_hex(
        const std::string &input,
        const std::unordered_set<char> &allowed_fillers)
    {
        ByteVector decoded;
        decoded.reserve(input.size() / 2);
        bool needs_shrinking = false;
        const char *high_nibble = nullptr;
        This::init_hex_values();

        for (auto it = input.begin(); it != input.end(); it++)
        {
            if (std::isxdigit(static_cast<unsigned char>(*it)))
            {
                if (high_nibble)
                {
                    decoded.push_back(
                        This::hex_values.at(*high_nibble) << 4 |
                        This::hex_values.at(*it));
                    high_nibble = nullptr;
                }
                else
                {
                    high_nibble = &*it;
                }
            }
            else if (allowed_fillers.count(*it))
            {
                needs_shrinking = true;
            }
            else
            {
                throwf(
                    std::invalid_argument,
                    "Invalid hexadecimal literal at input offset %d: '%c'",
                    std::distance(input.begin(), it),
                    *it);
            }
        }

        if (high_nibble)
        {
            throwf(std::invalid_argument,
                   "Hexadecimal string must contain an even number of digits, found %d",
                   decoded.size() * 2 + 1);
        }

        if (needs_shrinking)
        {
            decoded.shrink_to_fit();
        }

        return decoded;
    }

    void ByteVector::init_hex_values()
    {
        // On first invocation, initialize hex digit lookup table
        if (This::hex_values.empty())
        {
            This::hex_values.resize(2 << 8);
            for (const std::string &digits : This::hex_digits)
            {
                int value = 0;
                for (char digit : digits)
                {
                    This::hex_values.at(digit) = value++;
                }
            }
        }
    }

    void ByteVector::init_b64_values()
    {
        if (This::b64_values.empty())
        {
            This::b64_values.assign(256, -1);
            int value = 0;
            for (char symbol : This::b64_symbols)
            {
                This::b64_values[symbol] = value++;
            }
        }
    }

    std::vector<std::uint8_t> ByteVector::hex_values;
    const std::vector<std::string> ByteVector::hex_digits = {
        "0123456789abcdef",
        "0123456789ABCDEF",
    };

    std::vector<int> ByteVector::b64_values;
    const std::string ByteVector::b64_symbols =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
}  // namespace cc::core::types
