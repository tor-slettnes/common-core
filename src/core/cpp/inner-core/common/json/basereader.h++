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

namespace cc::json
{
    class BaseReader
    {
    protected:
        BaseReader(const std::string &name)
            : name(name)
        {
        }

    public:
        virtual types::Value decoded(const std::string &string) const = 0;
        virtual types::Value read_file(const fs::path &path) const = 0;
        virtual types::Value read_stream(std::istream &stream) const = 0;
        virtual types::Value read_stream(std::istream &&stream) const = 0;

    public:
        const std::string name;
    };
}  // namespace cc::json
