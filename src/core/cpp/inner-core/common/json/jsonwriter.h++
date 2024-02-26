/// -*- c++ -*-
//==============================================================================
/// @file jsonwriter.h++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "jsonencoder.h++"
#include "types/filesystem.h++"

#define RAPIDJSON_NO_SIZETYPEDEFINE
namespace rapidjson
{
    typedef ::std::size_t SizeType;
}

#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

namespace core::json
{
    using DefaultWriter = rapidjson::Writer<rapidjson::OStreamWrapper>;
    using PrettyWriter = rapidjson::PrettyWriter<rapidjson::OStreamWrapper>;

    //==========================================================================
    // JsonWriter

    template <class WriterType>
    class JsonWriterTemplate : JsonEncoder<WriterType>
    {
    public:
        JsonWriterTemplate(const fs::path &path)
            : stream_(std::make_shared<std::ofstream>(path))
        {
        }

        JsonWriterTemplate(std::ostream &&stream)
            : stream_(std::make_shared<std::ostream>(std::move(stream)))
        {
        }

        void write(const types::Value &value)
        {
            rapidjson::OStreamWrapper osw(*this->stream_);
            WriterType writer(osw);
            this->encodeValue(value, &writer);
            (*this->stream_) << std::endl;
        }

    protected:
        std::shared_ptr<std::ostream> stream_;
    };

    //==========================================================================
    // JsonPrettyWriter

    using JsonWriter = JsonWriterTemplate<DefaultWriter>;
    using JsonPrettyWriter = JsonWriterTemplate<PrettyWriter>;
}  // namespace core::json
