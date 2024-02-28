/// -*- c++ -*-
//==============================================================================
/// @file jsonwriter.c++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonwriter.h++"

namespace core::json
{
    JsonWriter::JsonWriter(const fs::path &path)
        : stream_(std::ofstream(path))
    {
    }

    void JsonWriter::write(const types::Value &value,
                           bool pretty)
    {
        JsonBuilder::to_stream(this->stream_, value, pretty);
    }

    void JsonWriter::write_to(const fs::path &path,
                              const types::Value &value,
                              bool pretty)

    {
        JsonBuilder::to_stream(std::ofstream(path), value, pretty);
    }

}  // namespace core::json
