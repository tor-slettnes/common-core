/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-base.h++"
#include "status/exceptions.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::parsers
{
    const int eof = std::char_traits<char>::eof();

    TokenParser::TokenParser(parsers::Input::ptr input,
                             const SymbolMapping &symbol_map)
        : input(input),
          symbol_map(symbol_map)
    {
    }

    TokenParser::TokenPair TokenParser::parse_spaces()
    {
        int c = this->input->getc();
        while (std::isspace(c))
        {
            this->input->append_to_token(c);
            c = this->input->getc();
        }
        this->input->ungetc(c);
        return {TI_SPACE, this->input->token()};
    }

    TokenParser::TokenPair TokenParser::parse_number()
    {
        int c = '\0';
        bool got_sign = (*this->input->token().begin() == '-');
        bool got_real = (*this->input->token().begin() == '.');
        bool got_hex = false;

        for (bool is_numeric = true; is_numeric;)
        {
            c = this->input->getc();
            switch (c)
            {
            case '0' ... '9':
                this->input->append_to_token(c);
                break;

            case '.':
            case 'e':
            case 'E':
                got_real = true;
                this->input->append_to_token(c);
                break;

            case 'x':
            case 'X':
                got_hex = true;
                this->input->append_to_token(c);
                break;

            default:
                is_numeric = false;
                break;
            }
        }
        this->input->ungetc(c);

        types::Value value;
        if (got_real)
        {
            return this->parse_numeric<double>();
        }
        else if (got_sign)
        {
            return this->parse_numeric<std::int64_t>();
        }
        else if (got_hex)
        {
            return this->parse_numeric<std::uint64_t>(0);
        }
        else
        {
            return this->parse_numeric<std::uint64_t>();
        }
    }

    TokenParser::TokenPair TokenParser::parse_symbol()
    {
        this->capture_identifier();
        try
        {
            return this->symbol_map.at(this->input->token());
        }
        catch (const std::out_of_range &e)
        {
            return {TI_INVALID, {}};
        }
    }

    TokenParser::TokenPair TokenParser::parse_line_comment()
    {
        int c = '\0';
        while ((c = this->input->getc()) != eof)
        {
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
            this->input->append_to_token(c);
        }
        return {TI_LINE_COMMENT, {}};
    }

    TokenParser::TokenPair TokenParser::parse_string(char quote, bool raw)
    {
        int c = '\0';
        bool escape = false;
        std::string string;
        std::size_t size = 0;
        std::size_t capacity = 64;
        string.reserve(capacity);

        while ((c = this->input->getc()) != eof)
        {
            this->input->append_to_token(c);

            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\' && !raw)
            {
                escape = true;
                continue;
            }
            else if (c == quote)
            {
                return {TI_QUOTED_STRING, string};
            }

            if (++size >= capacity)
            {
                capacity = std::max(std::size_t(64), 2 * capacity);
                string.reserve(capacity);
            }
            string.push_back(c);
        }

        return {TI_END, string};
    }

    void TokenParser::capture_identifier()
    {
        int c = '\0';
        for (bool is_symbolic = true; is_symbolic;)
        {
            c = this->input->getc();
            switch (c)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
            case '-':
            case '_':
                this->input->append_to_token(c);
                break;

            default:
                is_symbolic = false;
                break;
            }
        }
        this->input->ungetc(c);
    }

    char TokenParser::escape(char c)
    {
        switch (c)
        {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 'f':
            return '\f';
        case 'e':
            return '\x1b';
        default:
            return c;
        }
    }
}  // namespace core::parsers
