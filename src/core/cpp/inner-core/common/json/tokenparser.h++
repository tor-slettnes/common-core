/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.h++
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
        TI_NONE,
        TI_SPACE,
        TI_OBJECT_OPEN,
        TI_OBJECT_CLOSE,
        TI_ARRAY_OPEN,
        TI_ARRAY_CLOSE,
        TI_COMMA,
        TI_COLON,
        TI_NULL,
        TI_BOOL,
        TI_REAL,
        TI_SINT,
        TI_UINT,
        TI_STRING,
        TI_LINE_COMMENT,
        TI_UNKNOWN,
    };

    using TokenSet = std::unordered_set<TokenIndex>;

    class TokenParser
    {
    public:
        TokenParser(std::istream &stream);

        TokenIndex next_of(const TokenSet &candidates,
                           const TokenSet &endtokens = {});

        TokenIndex next_token();
        std::string token() const;

    private:
        TokenIndex token_index(char c, char last);
        TokenIndex parse_any(std::string &&token);
        TokenIndex parse_line_comment();
        TokenIndex parse_string();
        char escape(char c);
        void append_to_token(char c);

    private:
        std::istream &stream_;
        std::size_t pos_;
        std::string token_;
    };
}  // namespace core::json
