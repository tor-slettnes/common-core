/// -*- c++ -*-
//==============================================================================
/// @file basereader.h++
/// @brief Generic parser interface
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/filesystem.h++"

#include <iostream>
#include <string_view>

namespace core::parsers
{
    class BaseReader
    {
    protected:
        BaseReader(const std::string &name);
        virtual ~BaseReader();

    public:
        virtual std::optional<types::Value> try_decode(const std::string_view &text) const;
        virtual types::Value decoded(const std::string_view &text) const = 0;
        virtual types::Value read_file(const fs::path &path) const;
        virtual types::Value read_stream(std::istream &stream) const = 0;
        virtual types::Value read_stream(std::istream &&stream) const;

    public:
        const std::string name;
    };
}  // namespace core::parsers
