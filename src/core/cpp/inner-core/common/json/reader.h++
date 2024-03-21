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

namespace core::json
{
#if BUILD_RAPIDJSON
    using FastReader = RapidReader;
#else
    using FastReader = Reader;
#endif
}
