/// -*- c++ -*-
//==============================================================================
/// @file basewriter.c++
/// @brief JSON writer - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "basewriter.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

namespace core::json
{
    BaseWriter::BaseWriter()
    {
    }

    BaseWriter::BaseWriter(const fs::path &path)
        : stream_(std::make_unique<std::ofstream>(path))
    {
    }

    void BaseWriter::write(const types::Value &value, bool pretty)
    {
        assertf(this->stream_, "Cannot write JSON object with no output file");
        this->write_stream(*this->stream_, value, pretty);
    }

    void BaseWriter::append_file(const fs::path &path,
                                 const types::Value &value,
                                 bool pretty) const
    {
        std::ofstream fstream(path, std::ios_base::ate);
        this->write_stream(fstream, value, pretty);
    }

    void BaseWriter::write_file(const fs::path &path,
                                const types::Value &value,
                                bool pretty) const
    {
        std::ofstream fstream(path);
        this->write_stream(fstream, value, pretty);
    }

}  // namespace core::json
