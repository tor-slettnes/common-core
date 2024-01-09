// -*- c++ -*-
//==============================================================================
/// @file implementations.h++
/// @brief C++ demo - enumerate available implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/symbolmap.h++"

#include <map>
#include <iostream>

namespace cc::demo
{
    enum class Implementation
    {
        NONE,
        DDS,
        GRPC,
        ALL
    };

    extern cc::types::SymbolMap<Implementation> implementation_names;

    // Convert from the above enumeration to a string
    std::ostream &operator<<(std::ostream &stream, const Implementation &impl);

    // Convert to the above enumeration from a string
    std::istream &operator>>(std::istream &stream, Implementation &impl);
}  // namespace cc::demo
