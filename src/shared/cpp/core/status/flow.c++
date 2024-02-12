/// -*- c++ -*-
//==============================================================================
/// @file flow.c++
/// @brief Event flow
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "flow.h++"

namespace shared::status
{
    /// Names for printing.
    const types::SymbolMap<Flow> FlowNames = {
        {Flow::NONE, "NONE"},
        {Flow::PAUSED, "PAUSED"},
        {Flow::CANCELLED, "CANCELLED"},
        {Flow::ABORTED, "ABORTED"},
    };

    std::ostream &operator<<(std::ostream &stream, Flow flow)
    {
        try
        {
            stream << FlowNames.at(flow);
        }
        catch (const std::out_of_range &)
        {
            stream << "UNKNOWN_" << static_cast<uint>(flow);
        }
        return stream;
    }

    std::istream &operator>>(std::istream &stream, Flow &flow)
    {
        return FlowNames.from_stream(stream, &flow, Flow::NONE, false);
    }

}  // namespace shared::status
