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
        static const std::string hex_digits = "0123456789abcdef";

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
                stream << "\\x" << hex_digits.at(byte >> 4) << hex_digits.at(byte & 0xF);
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
        static const char code_table[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

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
            encoded.append(1, code_table[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, code_table[(temp & 0x0003F000) >> 12]);
            encoded.append(1, code_table[(temp & 0x00000FC0) >> 6]);
            encoded.append(1, code_table[(temp & 0x0000003F)]);
        }

        switch (size % 3)
        {
        case 1:
            temp = (*it++) << 16;
            encoded.append(1, code_table[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, code_table[(temp & 0x0003F000) >> 12]);
            encoded.append(2, pad_char);
            break;

        case 2:
            temp = (*it++) << 16;
            temp += (*it++) << 8;
            encoded.append(1, code_table[(temp & 0x00FC0000) >> 18]);
            encoded.append(1, code_table[(temp & 0x0003F000) >> 12]);
            encoded.append(1, code_table[(temp & 0x00000FC0) >> 6]);
            encoded.append(1, pad_char);
            break;
        }
        return encoded;
    }

    ByteVector ByteVector::from_base64(const std::string_view &input)
    {
        if (input.length() % 4 > 0)
        {
            throwf(std::invalid_argument,
                   "Invalid base64 string length %d, must be a multiple of 4",
                   input.size());
        }

        std::size_t padding = 0;
        for (std::string::size_type i = 0; i < 2; i++)
        {
            if ((input.length() > i) && input[input.length() - 1 - i] == pad_char)
            {
                padding++;
            }
        }

        ByteVector decoded;
        decoded.reserve(((input.length() / 4) * 3) - padding);

        for (auto it = input.begin(); it < input.end();)
        {
            std::uint32_t temp = 0;
            for (std::size_t i = 0; i < 4; i++)
            {
                temp <<= 6;
                if ('A' <= *it && *it <= 'Z')
                    temp |= *it - 0x41;
                else if ('a' <= *it && *it <= 'z')
                    temp |= *it - 0x47;
                else if ('0' <= *it && *it <= '9')
                    temp |= *it + 0x04;
                else if ('+' == *it)
                    temp |= 0x3E;
                else if ('/' == *it)
                    temp |= 0x3F;
                else if (pad_char == *it)
                {
                    switch (input.end() - it)
                    {
                    case 1:
                        decoded.push_back((temp >> 16) & 0x000000FF);
                        decoded.push_back((temp >> 8) & 0x000000FF);
                        return decoded;

                    case 2:
                        decoded.push_back((temp >> 10) & 0x000000FF);
                        return decoded;

                    default:
                        throwf(std::invalid_argument,
                               "Invalid padding length %d in base64!",
                               input.end() - it);
                    }
                }
                else
                {
                    throwf(std::invalid_argument,
                           "Invalid character '%c' in base64!",
                           *it);
                }
                ++it;
            }

            decoded.push_back((temp >> 16) & 0x000000FF);
            decoded.push_back((temp >> 8) & 0x000000FF);
            decoded.push_back(temp & 0x000000FF);
        }
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
        static const std::vector<std::string> hex_digits = {
            "0123456789abcdef",
            "0123456789ABCDEF",
        };
        const std::string &xdigits = hex_digits.at(uppercase);

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
        for (const char &c : input)
        {
            if (std::isxdigit(static_cast<unsigned char>(c)))
            {
                if (high_nibble)
                {
                    decoded.push_back(
                        This::digit_value(*high_nibble) << 4 |
                        This::digit_value(c));
                    high_nibble = nullptr;
                }
                else
                {
                    high_nibble = &c;
                }
            }
            else if (allowed_fillers.count(c))
            {
                needs_shrinking = true;
            }
            else
            {
                throwf(std::invalid_argument,
                       "Invalid hexadecimal literal: '%c'",
                       c);
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

    Byte ByteVector::digit_value(char c)
    {
        static std::vector<std::uint8_t> digit_values;

        // On first invocation, initialize hex digit lookup table
        if (digit_values.empty())
        {
            digit_values.resize(2 << 8);
            for (char c = '0'; c <= '9'; c++)
            {
                digit_values.at(c) = c - '0';
            }
            for (char c = 'A'; c <= 'F'; c++)
            {
                digit_values.at(c) = (c - 'A') + 0xA;
            }
            for (char c = 'a'; c <= 'f'; c++)
            {
                digit_values.at(c) = (c - 'a') + 0xA;
            }
        }
        return digit_values.at(static_cast<std::uint8_t>(c));
    }

}  // namespace cc::core::types
