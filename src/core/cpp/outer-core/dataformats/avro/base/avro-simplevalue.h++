/// -*- c++ -*-
//==============================================================================
/// @file avro-base.h++
/// @brief Common functionality wrappers for AVRO serialization
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-basevalue.h++"

#include "types/bytevector.h++"
#include "types/value.h++"

namespace avro
{
    class SimpleValue : public BaseValue
    {
        using This = SimpleValue;
        using Super = BaseValue;

    public:
        SimpleValue(bool boolean);
        SimpleValue(const char *input);
        SimpleValue(const std::string &input);
        SimpleValue(const std::string_view &input);
        SimpleValue(const core::types::Bytes &bytes);
    };
}
