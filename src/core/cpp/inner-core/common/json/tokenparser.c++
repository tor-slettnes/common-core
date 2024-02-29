/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser.h++"

#include "status/exceptions.h++"

#include <regex>
#include <string_view>

namespace core::json
{
    TokenParser::TokenParser(const std::string &text)
        : text_(text),
          match_start_(this->text_.begin()),
          match_end_(this->text_.begin())
    {
    }

    TokenIndex TokenParser::next_token()
    {
        static const std::vector<std::pair<TokenIndex, std::regex>> token_pairs = {
            {TI_OBJECT_OPEN, std::regex("^\\s*(\\{)")},
            {TI_OBJECT_CLOSE, std::regex("^\\s*(\\})")},
            {TI_ARRAY_OPEN, std::regex("^\\s*(\\[)")},
            {TI_ARRAY_CLOSE, std::regex("^\\s*(\\])")},
            {TI_COMMA, std::regex("^\\s*(\\,)")},
            {TI_COLON, std::regex("^\\s*(\\:)")},
            {TI_NULL, std::regex("^\\s*(null)")},
            {TI_BOOL, std::regex("^\\s*(true|false)")},
            {TI_REAL, std::regex("^\\s*([+-]?[0-9]+(?=[\\.eE])(?:\\.[0-9]*)?(?:[eE][+-]?[0-9]+)?)")},
            {TI_SINT, std::regex("^\\s*([+-][0-9]+)")},
            {TI_UINT, std::regex("^\\s*([0-9]+)")},
            {TI_STRING, std::regex("^\\s*(\"((?:\\\\.|[^\"\\\\\\r\\n])*)\")")},
            {TI_COMMENT, std::regex("^\\s*((?://|#)[^\\r\\n]*)")},
            {TI_UNKNOWN, std::regex("^\\s*(\\S+)")},
        };

        std::smatch match;
        std::string::const_iterator it = this->match_end_;
        for (const auto &[ti, rx] : token_pairs)
        {
            if (std::regex_search(it, this->text_.end(), match, rx))
            {
                this->match_start_ = it + match.position(1);
                this->match_end_ = this->match_start_ + match.length(1);

                std::size_t inner_group = match.size()-1;
                it += match.position(inner_group);
                this->token_ = std::string_view(&*it, match.length(inner_group));

                return ti;
            }
        }

        // this->match_start_ = this->match_end_ = this->text_.end();
        this->token_ = std::string_view();
        return TI_NONE;
    }

    TokenIndex TokenParser::next_of(const TokenSet &candidates,
                                    const TokenSet &endtokens)
    {
        TokenIndex index = this->next_token();
        while (index == TI_COMMENT)
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
                   "Unexpected JSON token at position %d: %s",
                   std::distance(this->text_.begin(), this->match_start()),
                   std::string(this->match_start(), this->match_end()));
        }
    }

    std::string::const_iterator TokenParser::match_start() const
    {
        return this->match_start_;
    }

    std::string::const_iterator TokenParser::match_end() const
    {
        return this->match_end_;
    }

    std::string TokenParser::token() const
    {
        return std::string(this->token_);
    }

}  // namespace core::json
