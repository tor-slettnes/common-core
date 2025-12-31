/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.h++
/// @brief Iterate over interesting JSON tokens
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "parserinput-base.h++"
#include "types/value.h++"

#include <charconv>

namespace core::json
{
    class TokenParser
    {
    public:
        enum TokenIndex
        {
            TI_NONE = 0x0000,

            TI_INVALID = 0x0001,
            TI_SPACE = 0x0002,
            TI_COMMA = 0x0004,
            TI_COLON = 0x0008,

            TI_LINE_COMMENT = 0x0010,
            TI_QUOTED_STRING = 0x0020,
            TI_SYMBOL = 0x0040,
            TI_NUMERIC = 0x0080,

            TI_MAP_OPEN = 0x0100,
            TI_MAP_CLOSE = 0x0200,
            TI_LIST_OPEN = 0x0400,
            TI_LIST_CLOSE = 0x0800,

            TI_NULLVALUE = 0x1000,
            TI_BOOL = 0x2000,
            TI_END = 0x8000
        };

        using TokenMask = std::uint64_t;
        using TokenPair = std::pair<TokenIndex, types::Value>;
        using CommentStyles = std::vector<std::string_view>;
        using SymbolMapping = std::unordered_map<std::string_view, TokenPair>;
        using ptr = std::shared_ptr<TokenParser>;

    public:
        TokenParser(parsers::Input::ptr input);

        TokenPair next_of(const TokenMask &expected,
                          const TokenMask &endtokens = TI_NONE);

    protected:
        TokenPair next_token();
        TokenIndex token_index(int c) const;

        TokenPair parse_spaces();
        TokenPair parse_number();
        TokenPair parse_symbol();
        TokenPair parse_line_comment();
        TokenPair parse_string(char quote, bool raw = false);

        void capture_identifier();
        char escape(char c);

    private:
        template <class T, class... Args>
        TokenPair parse_numeric(Args &&...args);

    protected:
        parsers::Input::ptr input;

    private:
        static const SymbolMapping symbol_map;
        std::list<std::string> indentation_stack;
    };
} // namespace core::json
