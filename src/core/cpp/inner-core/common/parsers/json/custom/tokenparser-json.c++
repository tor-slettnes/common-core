/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-json.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::json
{
    const int eof = std::char_traits<char>::eof();

    JsonParser::JsonParser(parsers::Input::ptr input)
        : Super(input,                   // input
                {"#", "//"},             // comment_syles
                JsonParser::symbol_map)  // symbol_map
    {
    }

    parsers::TokenParser::TokenPair JsonParser::next_token()
    {
        int c = this->input->getc();

        while (TokenIndex ti = this->token_index(c))
        {
            if (ti == TI_SPACE)
            {
                c = this->input->getc();
            }
            else
            {
                this->input->init_token(c);
                return {ti, {}};
            }
        }

        this->input->init_token(c);
        switch (c)
        {
        case '#':
        case '/':
            return this->parse_line_comment();

        case '"':
        case '\'':
            return this->parse_string(c);

        case '-':
        case '0' ... '9':
            return this->parse_number();

        case 'a' ... 'z':
        case 'A' ... 'Z':
            return this->parse_symbol();

        default:
            return {TI_INVALID, {}};
        }
    }

    parsers::TokenParser::TokenIndex JsonParser::token_index(int c) const
    {
        switch (c)
        {
        case std::char_traits<char>::eof():
            return TI_END;

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

        default:
            return TI_NONE;
        }
    }

    parsers::TokenParser::SymbolMapping JsonParser::symbol_map = {
        {"null", {TI_NULLVALUE, types::Value()}},
        {"false", {TI_BOOL, types::Value(false)}},
        {"true", {TI_BOOL, types::Value(true)}},
    };

}  // namespace core::json
