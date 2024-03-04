/// -*- c++ -*-
//==============================================================================
/// @file jsonencoder.h++
/// @brief Encode variant values as rapidjson objects
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"

namespace core::json
{
    /// @brief Encode `types::Value()` & friends to rapidJson writer instances
    /// @tparam[in] WriterType
    ///      The JsonWriter type to use: `Writer` or `PrettyWriter`.

    template <class WriterType>
    class JsonEncoder : protected WriterType
    {
    public:
        bool encodeValue(const types::Value &value, WriterType *writer);
        bool encodeArray(const types::ValueList &list, WriterType *writer);
        bool encodeObject(const types::KeyValueMap &kvmap, WriterType *writer);
        bool encodeObject(const types::TaggedValueList &tvlist, WriterType *writer);
    };

}  // namespace core::json

#include "jsonencoder.i++"
