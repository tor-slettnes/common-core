/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parsers/common/basewriter.h++"

namespace core::json
{
    class CustomWriter : public parsers::BaseWriter
    {
        using This = CustomWriter;
        using Super = parsers::BaseWriter;

    public:
        CustomWriter();
        CustomWriter(const fs::path &path);

        void write_stream(std::ostream &stream,
                          const types::Value &value,
                          bool pretty = false) const override;

        std::string encoded(const types::Value &value,
                            bool pretty = false) const override;

    private:
        static std::ostream &to_stream(std::ostream &stream,
                                       const types::Value &value,
                                       bool pretty = false,
                                       const std::string &indent = "");

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::KeyValueMap &kvmap,
                                       bool pretty,
                                       const std::string &indent);

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::ValueList &list,
                                       bool pretty,
                                       const std::string &indent);

        static std::ostream &to_stream(std::ostream &stream,
                                       const types::TaggedValueList &tvlist,
                                       bool pretty,
                                       const std::string &indent);
    };
}  // namespace core::json
