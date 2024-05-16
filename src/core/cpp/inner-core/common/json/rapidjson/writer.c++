/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "writer.h++"
#include "builder.h++"

namespace cc::json
{

    RapidWriter::RapidWriter()
        : Super("RapidWriter")
    {
    }

    RapidWriter::RapidWriter(const fs::path &path)
        : Super("RapidWriter", path)
    {
    }

    void RapidWriter::write_stream(std::ostream &stream,
                                   const types::Value &value,
                                   bool pretty) const
    {
        RapidBuilderRef builder;
        if (pretty)
        {
            builder = std::make_shared<DefaultBuilder>(stream);
        }
        else
        {
            builder = std::make_shared<PrettyBuilder>(stream);
        }

        builder->write_stream(value);
    }

    std::string RapidWriter::encoded(const types::Value &value,
                                     bool pretty) const
    {
        std::stringstream ss;
        this->write_stream(ss, value, pretty);
        return ss.str();
    }

}  // namespace cc::json
