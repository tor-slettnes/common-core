/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "builder.h++"
#include "parsers/common/basewriter.h++"

namespace cc::core::json
{
    class RapidWriter : public parsers::BaseWriter
    {
        using This = RapidWriter;
        using Super = parsers::BaseWriter;

    public:
        RapidWriter();
        RapidWriter(const fs::path &path,
                    std::ios_base::openmode mode = std::ios_base::out);

        void write_stream(std::ostream &stream,
                          const types::Value &value,
                          bool pretty = false) const override;

        std::string encoded(const types::Value &value,
                            bool pretty = false) const override;
    };

}  // namespace cc::core::json
