/// -*- c++ -*-
//==============================================================================
/// @file basewriter.c++
/// @brief JSON writer - abstract interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "basewriter.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

namespace cc::json
{
    BaseWriter::BaseWriter(const std::string &name)
        : name(name)
    {
    }

    BaseWriter::BaseWriter(const std::string &name, const fs::path &path)
        : name(name),
          stream_(std::make_unique<std::ofstream>(path))
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

}  // namespace cc::json
