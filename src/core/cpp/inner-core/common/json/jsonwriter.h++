/// -*- c++ -*-
//==============================================================================
/// @file jsonwriter.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "jsonbuilder.h++"
#include "types/filesystem.h++"

#include <fstream>

namespace core::json
{
    class JsonWriter
    {
        using This = JsonWriter;

    public:
        JsonWriter(const fs::path &path);
        void write(const types::Value &value, bool pretty = true);

        static void write_to(const fs::path &path,
                             const types::Value &value,
                             bool pretty = true);

    protected:
        std::ofstream stream_;
    };
}  // namespace core::json
