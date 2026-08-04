// -*- c++ -*-
//==============================================================================
/// @file relay-types.c++
/// @brief Relay common definitions
/// @author Tor Slettnes
//==============================================================================

#include "relay-types.h++"
#include "string/format.h++"

namespace cc::platform::pubsub
{
    std::ostream &operator<<(std::ostream& stream, const ReplayPolicy& policy)
    {
        core::str::format(stream,
                          "{replay_latest=%b, mapping_keys=%s}",
                          policy.replay_latest,
                          policy.mapping_keys);
        return stream;
    }

    //--------------------------------------------------------------------------
    // Signals

    core::signal::MappingSignal<core::types::Value> signal_publication(
        "signal_publication",
        false);

}  // namespace cc::platform::pubsub
