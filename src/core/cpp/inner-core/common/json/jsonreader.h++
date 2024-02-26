/// -*- c++ -*-
//==============================================================================
/// @file jsonreader.h++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "jsondecoder.h++"
#include "types/value.h++"
#include "types/filesystem.h++"

namespace core::json
{
    class JsonReader
    {
        using This = JsonReader;

    public:
        JsonReader(const fs::path &path);

        types::Value read();

        static types::Value read_from(const fs::path &path);

    private:
        static std::string read_text(const fs::path &path);

    private:
        fs::path path_;
    };
}  // namespace core::json
