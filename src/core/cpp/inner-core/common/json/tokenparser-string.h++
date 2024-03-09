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
        StringParser(const std::string &string);

        std::size_t token_position() const override;
        std::string token() const override;
        std::string::const_iterator token_begin() const override;
        std::string::const_iterator token_end() const override;

        int getc() override;
        void ungetc(int c) override;
        void init_token(char c) override;

    private:
        const std::string &string_;
        std::string::const_iterator it_;
        const std::string::const_iterator end_;
        std::string::const_iterator token_start_;
    };
}  // namespace core::json
