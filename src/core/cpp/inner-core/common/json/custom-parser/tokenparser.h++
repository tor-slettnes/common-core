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
        TokenParser(const std::string &string);
        ~TokenParser();

        TokenIndex next_of(const TokenSet &candidates,
                           const TokenSet &endtokens = {});

        TokenIndex next_token();
        const std::string &token() const;
        const types::Value &value() const;

    private:
        template <class T, class... Args>
        TokenIndex parse_number(TokenIndex ti, const std::string &type, Args &&...args);

        TokenIndex parse_real();
        TokenIndex parse_sint();
        TokenIndex parse_uint(int base = 10);
        TokenIndex parse_symbol();
        TokenIndex parse_line_comment();
        TokenIndex parse_string();
        char escape(char c);

        void append_to_token(char c);
        bool got_token() const;

    private:
        const std::string &string_;
        std::string::const_iterator it_;
        const std::string::const_iterator end_it_;
        std::string::const_iterator token_start_;
        std::string::const_iterator token_end_;
        std::string token_;
        types::Value value_;

    public:
        dt::Duration string_parse_time_;
        dt::Duration any_parse_time_;
    };
}  // namespace core::json
