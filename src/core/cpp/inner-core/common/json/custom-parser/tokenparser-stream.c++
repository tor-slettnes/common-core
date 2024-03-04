/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>

namespace core::json
{
    TokenParser::TokenParser(std::istream &stream)
        : stream_(stream),
          pos_(0),
          string_parse_time_(dt::Duration::zero()),
          any_parse_time_(dt::Duration::zero())
    {
        logf_debug("TokenParser created");

        // Enough to contain most miscellaneous content;
        // expanded by powers of two when parsing longer strings.
        this->token_.reserve(64);
    }

    TokenParser::~TokenParser()
    {
        logf_debug("TokenParser spent %s parsing strings, %s parsing other types",
                   this->string_parse_time_,
                   this->any_parse_time_);
    }

    TokenIndex TokenParser::next_of(const TokenSet &candidates,
                                    const TokenSet &endtokens)
    {
        TokenIndex index = this->next_token();
        while (index == TI_LINE_COMMENT)
        {
            index = this->next_token();
        }

        if (candidates.count(index))
        {
            return index;
        }
        else if (endtokens.count(index))
        {
            return TI_NONE;
        }
        else if (index == TI_NONE)
        {
            throwf(exception::MissingArgument,
                   "Missing JSON token at end of input");
        }
        else
        {
            throwf(exception::InvalidArgument,
                   "Unexpected JSON token type %d: %r",
                   index,
                   this->token());
        }
    }

    TokenIndex TokenParser::next_token()
    {
        char c;
        bool unknowns = false;
        this->token_.clear();   // Does not change capacity

        for (this->stream_.get(c);
             this->stream_.good();
             this->stream_.get(c))
        {
            TokenIndex ti = this->token_index(c);
            if (unknowns && (ti != TI_UNKNOWN))
            {
                this->stream_.unget();
                return this->parse_any();
            }

            switch (ti)
            {
            case TI_SPACE:
                continue;

            case TI_STRING:
                return this->parse_string();

            case TI_LINE_COMMENT:
                this->parse_line_comment();
                this->token_.clear();
                unknowns = false;
                break;

            case TI_UNKNOWN:
                unknowns = true;
                this->token_.push_back(c);
                break;

            default:
                return ti;
            }
        }

        if (unknowns)
        {
            return this->parse_any();
        }
        else
        {
            return TI_NONE;
        }
    }

    TokenIndex TokenParser::token_index(char c)
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

    TokenIndex TokenParser::parse_any()
    {
        steady::TimePoint start = steady::Clock::now();

        static const std::map<std::string, TokenIndex> const_map = {
            {"null", TI_NULL},
            {"true", TI_BOOL},
            {"false", TI_BOOL},
        };

        static const std::regex rx(
            "([0-9]+)"                                                     // (1) unsigned int
            "|([+-][0-9]+)"                                                // (2) signed int
            "|([+-]?[0-9]+(?=[\\.eE])(?:\\.[0-9]*)?(?:[eE][+-]?[0-9]+)?)"  // (3) real
            "|((?://|#).*)"                                                // (4) comment
        );

        TokenIndex ti = TI_UNKNOWN;

        if (auto it = const_map.find(this->token_); it != const_map.end())
        {
            ti = it->second;
        }

        else if (std::smatch match; std::regex_match(this->token_, match, rx))
        {
            ti = match.length(1)   ? TI_UINT
                 : match.length(2) ? TI_SINT
                 : match.length(3) ? TI_REAL
                 : match.length(4) ? this->parse_line_comment()
                                   : TI_UNKNOWN;
        }

        this->any_parse_time_ += (steady::Clock::now() - start);
        return ti;
    }

    TokenIndex TokenParser::parse_line_comment()
    {
        char c = '\0';
        for (this->stream_.get(c);
             this->stream_.good();
             this->stream_.get(c))
        {
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
        }
        return TI_LINE_COMMENT;
    }

    TokenIndex TokenParser::parse_string()
    {
        steady::TimePoint start = steady::Clock::now();
        char c = '\0';
        bool escape = false;
        this->token_.clear();
        std::size_t size = this->token_.size();
        std::size_t capacity = this->token_.capacity();

        for (this->stream_.get(c);
             this->stream_.good();
             this->stream_.get(c))
        {
            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\')
            {
                escape = true;
                continue;
            }
            else if (c == '"')
            {
                this->string_parse_time_ += (steady::Clock::now() - start);
                return TI_STRING;
            }

            if (size >= capacity)
            {
                this->token_.reserve(std::max(std::size_t(64), 2 * capacity));
                capacity = this->token_.capacity();
            }

            this->token_.push_back(c);
            size++;
        }
        this->string_parse_time_ += (steady::Clock::now() - start);
        return TI_NONE;
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

    std::string TokenParser::token() const
    {
        return std::string(this->token_);
    }

}  // namespace core::json
