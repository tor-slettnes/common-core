/// -*- c++ -*-
//==============================================================================
/// @file writer.h++
/// @brief JSON writer implementations
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "custom/writer.h++"

#if BUILD_RAPIDJSON
#include "rapidjson/writer.h++"
#endif

namespace core::json
{
    using Writer = CustomWriter;

#if BUILD_RAPIDJSON
    using FastWriter = RapidWriter;
#else
    using FastWriter = CustomWriter;
#endif

    extern Writer writer;
    extern FastWriter fast_writer;
}  // namespace core::json
