/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.h++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include <unordered_set>

namespace core::json
{
    enum TokenIndex
    {
        TI_NONE = 0x0000,
        TI_INVALID = 0x0001,
        TI_SPACE = 0x0002,
        TI_OBJECT_OPEN = 0x0004,
        TI_OBJECT_CLOSE = 0x0009,
        TI_ARRAY_OPEN = 0x0010,
        TI_ARRAY_CLOSE = 0x0020,
        TI_COMMA = 0x0040,
        TI_COLON = 0x0080,
        TI_NULL = 0x0100,
        TI_BOOL = 0x0200,
        TI_NUMERIC = 0x0400,
        TI_STRING = 0x0800,
        TI_LINE_COMMENT = 0x4000,
        TI_END = 0x8000
    };

    using TokenMask = std::uint64_t;
    using TokenPair = std::pair<TokenIndex, core::types::Value>;

    class TokenParser
    {
    public:
        TokenPair next_of(const TokenMask &expected,
                          const TokenMask &endtokens = TI_NONE);

        TokenPair next_token();
        virtual std::size_t token_position() const = 0;
        virtual std::string token() const = 0;
        virtual std::string::const_iterator token_begin() const = 0;
        virtual std::string::const_iterator token_end() const = 0;

    protected:
        virtual int getc() = 0;
        virtual void ungetc(int c) = 0;
        virtual void init_token(char c) = 0;
        virtual void append_to_token(char c) {}

    private:
        TokenIndex token_index(int c) const;

        template <class T, class... Args>
        TokenPair parse_numeric(Args &&...args);
        TokenPair parse_number();
        TokenPair parse_symbol();
        TokenPair parse_line_comment();
        TokenPair parse_string(char quote);
        char escape(char c);
    };
}  // namespace core::json
