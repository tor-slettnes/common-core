/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-yaml.h++
/// @brief Iterate over interesting YAML tokens
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "parsers/common/tokenparser-base.h++"

namespace core::yaml
{
    class YamlParser : public parsers::TokenParser
    {
        using This = YamlParser;
        using Super = parsers::TokenParser;

    public:
        YamlParser(parsers::Input::ptr input);

        TokenPair parse_document();
        TokenPair parse_indentation();

        TokenPair parse_indentation();
        TokenPair parse_key();
        TokenPair parse_value();

        TokenPair next_token() override;
        TokenIndex token_index(int c) const override;

    protected:
        TokenPair parse_spaces();
        TokenPair parse_symbol() override;

        void set_indendation(const std::string &indentation);

    private:
        std::size_t line_indent;
        std::list<TokenPair> indentation_stack;

    };
}  // namespace core::yaml
