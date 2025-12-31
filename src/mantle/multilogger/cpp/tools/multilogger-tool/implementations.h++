// -*- c++ -*-
//==============================================================================
/// @file implementations.h++
/// @brief Enumerate available MultiLogger API implementations
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/symbolmap.h++"

namespace multilogger
{
    enum class Implementation
    {
        NONE,
        GRPC,
        ZMQ
    };

    extern core::types::SymbolMap<Implementation> implementation_names;

    // Convert from the above enumeration to a string
    std::ostream &operator<<(std::ostream &stream, const Implementation &impl);

    // Convert to the above enumeration from a string
    std::istream &operator>>(std::istream &stream, Implementation &impl);
}
