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
        TI_COMMENT,
        TI_UNKNOWN,
    };

    using TokenSet = std::unordered_set<TokenIndex>;

    class TokenParser
    {
    public:
        TokenParser(const std::string &text);

        TokenIndex next_token();

        TokenIndex next_of(const TokenSet &candidates,
                           const TokenSet &endtokens = {});

        std::string token() const;
        std::string::const_iterator match_start() const;
        std::string::const_iterator match_end() const;

    private:
        const std::string &text_;
        std::string::const_iterator match_start_, match_end_;
        std::string_view token_;
    };
}  // namespace core::json
