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
        std::string token() const;

    private:
        inline TokenIndex token_index(char c)
        {
            switch (c)
            {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case '\v':
            case '\f':
                return TI_SPACE;

            case '{':
                return TI_OBJECT_OPEN;

            case '}':
                return TI_OBJECT_CLOSE;

            case '[':
                return TI_ARRAY_OPEN;

            case ']':
                return TI_ARRAY_CLOSE;

            case ',':
                return TI_COMMA;

            case ':':
                return TI_COLON;

            case '"':
                return TI_STRING;

            case '#':
                return TI_LINE_COMMENT;
            }

            return TI_UNKNOWN;
        }

        TokenIndex parse_any();
        TokenIndex parse_line_comment();
        TokenIndex parse_string();
        char escape(char c);
        void append_to_token(char c);

    private:
        const std::string &string_;
        std::string::const_iterator it_;
        const std::string::const_iterator end_it_;
        std::string::const_iterator token_start_;
        std::string::const_iterator token_end_;
        std::string token_;

    public:
        dt::Duration string_parse_time_;
        dt::Duration any_parse_time_;
    };
}  // namespace core::json
