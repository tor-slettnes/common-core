/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-string.h++
/// @brief JSON token iterator for string inputs
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tokenparser-base.h++"

namespace core::json
{
    class StringParser : public TokenParser
    {
    public:
        StringParser(const std::string_view &string);

        std::size_t token_position() const override;
        std::string_view token() const override;

        int getc() override;
        void ungetc(int c) override;
        void init_token(char c) override;

    private:
        std::string_view string_;
        std::size_t pos_, token_pos_;
    };
}  // namespace core::json
