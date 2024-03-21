/// -*- c++ -*-
//==============================================================================
/// @file basereader.h++
/// @brief JSON parser - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "types/filesystem.h++"

#include <iostream>

namespace core::json
{
    class BaseReader
    {
    public:
        virtual types::Value decoded(const std::string &string) const = 0;
        virtual types::Value read_file(const fs::path &path) const = 0;
        virtual types::Value read_stream(std::istream &stream) const = 0;
        virtual types::Value read_stream(std::istream &&stream) const = 0;
    };
}  // namespace core::json
