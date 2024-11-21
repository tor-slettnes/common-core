/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief JSON parser implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "custom/reader.h++"

#if BUILD_RAPIDJSON
#include "rapidjson/reader.h++"
#endif

namespace core::json
{
    using Reader = CustomReader;

#if BUILD_RAPIDJSON
    using FastReader = RapidReader;
#else
    using FastReader = CustomReader;
#endif

    extern Reader reader;
    extern FastReader fast_reader;
}  // namespace core::json
