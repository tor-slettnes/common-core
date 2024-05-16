/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief JSON parser implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "custom-parser/reader.h++"

#if BUILD_RAPIDJSON
#include "rapidjson/reader.h++"
#endif

namespace cc::json
{
    using Reader = CustomReader;

#if BUILD_RAPIDJSON
    using FastReader = RapidReader;
#else
    using FastReader = CustomReader;
#endif

    extern Reader reader;
    extern FastReader fast_reader;
}
