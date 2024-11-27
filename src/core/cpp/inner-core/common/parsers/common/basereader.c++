/// -*- c++ -*-
//==============================================================================
/// @file basereader.c++
/// @brief Generic parser interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "basereader.h++"

#include <fstream>

namespace core::parsers
{
    BaseReader::BaseReader(const std::string &name)
        : name(name)
    {
    }

    BaseReader::~BaseReader()
    {
    }

    types::Value BaseReader::read_file(const fs::path &path) const
    {
        if (std::ifstream is{path})
        {
            return this->read_stream(is);
        }
        else
        {
            return {};
        }
    }

    types::Value BaseReader::read_stream(std::istream &&stream) const
    {
        return this->read_stream(stream);
    }
}  // namespace core::parsers
