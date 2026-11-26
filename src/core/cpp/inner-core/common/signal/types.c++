/// -*- c++ -*-
//==============================================================================
/// @file types.c++
/// @brief Adaptation of Signal/Slot pattern - type definitions
/// @author Tor Slettnes
//==============================================================================

#include "signal/types.h++"
#include "types/symbolmap.h++"

namespace cc::core::signal
{
    const types::SymbolMap<MappingAction> action_name_map = {
        {MAP_NONE, "NONE"},
        {MAP_ADDITION, "ADDITION"},
        {MAP_REMOVAL, "REMOVAL"},
        {MAP_UPDATE, "UPDATE"},
    };

    std::ostream &operator<<(std::ostream &stream, MappingAction action)
    {
        return action_name_map.to_stream(stream, action);
    }

    std::istream &operator>>(std::istream &stream, MappingAction &action)
    {
        return action_name_map.from_stream(stream, &action);
    }
}  // namespace cc::core::signal
