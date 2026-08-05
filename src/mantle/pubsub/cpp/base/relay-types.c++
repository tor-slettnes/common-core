// -*- c++ -*-
//==============================================================================
/// @file relay-types.c++
/// @brief Relay common definitions
/// @author Tor Slettnes
//==============================================================================

#include "relay-types.h++"
#include "string/misc.h++"
#include "string/format.h++"

namespace cc::platform::pubsub
{
    std::ostream& operator<<(std::ostream& stream, const ReplayPolicy& policy)
    {
        std::vector<std::string> key_paths;
        key_paths.reserve(policy.key_paths.size());
        for (const auto& path : policy.key_paths)
        {
            key_paths.push_back(core::str::join(path, KEY_PATH_DELIMITER));
        }

        core::str::format(stream,
                          "{replay=%b, key_paths=%s}",
                          policy.replay_latest,
                          key_paths);
        return stream;
    }

    //--------------------------------------------------------------------------
    // Signals

    core::signal::MappingSignal<core::types::Value> signal_publication(
        "signal_publication",
        false);

}  // namespace cc::platform::pubsub
