/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.h++
/// @brief Iterate over interesting tokens in provided text input
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parserinput-base.h++"
#include "types/value.h++"

#include <charconv>

namespace core::parsers
{
    class TokenParser
    {
    public:
        enum TokenIndex
        {
            TI_NONE = 0x0000,
            TI_INVALID = 0x0001,
            TI_SPACE = 0x0002,
            TI_OBJECT_OPEN = 0x0004,
            TI_OBJECT_CLOSE = 0x0008,
            TI_ARRAY_OPEN = 0x0010,
            TI_ARRAY_CLOSE = 0x0020,
            TI_COMMA = 0x0040,
            TI_COLON = 0x0080,
            TI_NULLVALUE = 0x0100,
            TI_BOOL = 0x0200,
            TI_NUMERIC = 0x0400,
            TI_STRING = 0x0800,
            TI_LINE_COMMENT = 0x40000000,
            TI_END = 0x80000000
        };

        using TokenMask = std::uint64_t;
        using TokenPair = std::pair<TokenIndex, types::Value>;
        using SymbolMapping = std::unordered_map<std::string_view, TokenPair>;
        using CommentStyles = std::vector<std::string_view>;
        using ptr = std::shared_ptr<TokenParser>;

    public:
        TokenParser(parsers::Input::ptr input,
                    const CommentStyles &comment_styles,
                    const SymbolMapping &symbol_map);

        TokenPair next_of(const TokenMask &expected,
                          const TokenMask &endtokens = TI_NONE);

    protected:
        virtual TokenPair next_token() = 0;
        virtual TokenIndex token_index(int c) const = 0;

    protected:
        TokenPair parse_number();
        TokenPair parse_symbol();
        TokenPair parse_line_comment();
        TokenPair parse_string(char quote, bool raw = false);
        char escape(char c);
        bool is_comment();

    protected:
        template <class T, class... Args>
        TokenPair parse_numeric(Args &&...args)
        {
            static const std::errc ok{};
            const char *const start = &*this->input->token().begin();
            const char *const end = &*this->input->token().end();
            T number = 0;

            auto [ptr, ec] = std::from_chars(start, end, number, args...);
            if (ec == ok)
            {
                return {TI_NUMERIC, number};
            }
            else
            {
                return {TI_INVALID, {}};
            }
        }

    protected:
        parsers::Input::ptr input;

    private:
        CommentStyles comment_styles;
        const SymbolMapping &symbol_map;
    };
}  // namespace core::parsers
