/// -*- c++ -*-
//==============================================================================
/// @file builder.h++
/// @brief Encode variant values as rapidjson objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
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
    //==========================================================================
    // Untyped builder base
    class RapidBuilderBase
    {
    protected:
        RapidBuilderBase(std::ostream &stream)
            : stream(stream)
        {
        }

    public:
        virtual void write_stream(const types::Value &value) = 0;

    protected:
        std::ostream &stream;
    };

    //==========================================================================
    // Builder base with specific RapidJSON writer type

    template <class WriterType>
    class RapidBuilder : public RapidBuilderBase
    {
        using Super = RapidBuilderBase;

    public:
        RapidBuilder(std::ostream &stream);
        void write_stream(const types::Value &value) override;
        bool encodeValue(const types::Value &value);
        bool encodeArray(const types::ValueList &list);
        bool encodeObject(const types::KeyValueMap &kvmap);
        bool encodeObject(const types::TaggedValueList &tvlist);

    private:
        rapidjson::OStreamWrapper streamwrapper;
        WriterType writer;
    };

    //==========================================================================
    // Convenience aliases

    using DefaultWriter = ::rapidjson::Writer<::rapidjson::OStreamWrapper>;
    using PrettyWriter = ::rapidjson::PrettyWriter<::rapidjson::OStreamWrapper>;

    using DefaultBuilder = RapidBuilder<DefaultWriter>;
    using PrettyBuilder = RapidBuilder<PrettyWriter>;

}  // namespace core::json

#include "builder.i++"
