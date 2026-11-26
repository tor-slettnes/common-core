/// -*- c++ -*-
//==============================================================================
/// @file types.h++
/// @brief Adaptation of Signal/Slot pattern - type definitions
/// @author Tor Slettnes
//==============================================================================

#pragma once

#include <string>
#include <vector>
#include <future>
#include <iostream>

namespace cc::core::signal
{
    using Handle = std::string;
    using Futures = std::vector<std::future<bool>>;

    // Synchronized to `picarro.protobuf.signal.MappingAction` in `signal.proto`.
    // We do not use that here, since ProtoBuf support is optional, not part of
    // the "inner core".

    enum MappingAction
    {
        MAP_NONE,
        MAP_ADDITION,
        MAP_REMOVAL,
        MAP_UPDATE,
    };

    //==========================================================================
    // I/O stream support

    std::ostream &operator<<(std::ostream &stream, MappingAction action);
    std::istream &operator>>(std::istream &stream, MappingAction &action);
}  // namespace cc::core::signal
