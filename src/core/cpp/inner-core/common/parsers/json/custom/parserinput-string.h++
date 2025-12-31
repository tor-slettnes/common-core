/// -*- c++ -*-
//==============================================================================
/// @file parserinput-string.h++
/// @brief Read tokens from strings
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "parserinput-base.h++"

namespace core::parsers
{
    class StringInput : public Input
    {
    public:
        StringInput(const std::string_view &string);

        std::size_t token_position() const override;
        std::string_view token() const override;

        int getc() override;
        void ungetc(int c) override;
        void init_token() override;
        void init_token(char c) override;
        void append_to_token(char c) override;

    private:
        std::string_view string_;
        std::size_t pos_, token_pos_;
    };
}  // namespace core::parsing
