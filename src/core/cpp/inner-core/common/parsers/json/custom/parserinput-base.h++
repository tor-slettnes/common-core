/// -*- c++ -*-
//==============================================================================
/// @file parserinput-base.h++
/// @brief Abstract source for text fed to parser
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string_view>
#include <memory>

namespace core::parsers
{
    class Input
    {
    public:
        using ptr = std::shared_ptr<Input>;

    public:
        virtual std::size_t token_position() const = 0;
        virtual std::string_view token() const = 0;
        virtual int getc() = 0;
        virtual void ungetc(int c) = 0;
        virtual void init_token() = 0;
        virtual void init_token(char c) = 0;
        virtual void append_to_token(char c) = 0;
    };
}  // namespace core::parser
