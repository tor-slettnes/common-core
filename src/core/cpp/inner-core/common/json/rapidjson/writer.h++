/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "builder.h++"
#include "json/basewriter.h++"

namespace core::json
{
    class RapidWriter : public BaseWriter
    {
        using This = RapidWriter;
        using Super = BaseWriter;

    public:
        using Super::Super;

        void write_file(const fs::path &path,
                        const types::Value &value,
                        bool pretty = false) const override;

        void write_stream(std::ostream &stream,
                          const types::Value &value,
                          bool pretty = false) const override;

        std::string encoded(const types::Value &value,
                            bool pretty = false) const override;
    };

}  // namespace core::json
