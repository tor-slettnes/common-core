// -*- c++ -*-
//==============================================================================
/// @file protobuf-relay-types.h++
/// @brief conversions to/from Protocol Buffer messages in "pubsub_types.proto"
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-relay-types.h++"
#include "protobuf-inline.h++"

namespace cc::protobuf
{
    //--------------------------------------------------------------------------
    // ReplayPolicy
    void encode(const cc::platform::pubsub::ReplayPolicy& native,
                cc::platform::pubsub::protobuf::ReplayPolicy* proto)
    {
        proto->set_replay_latest(native.replay_latest);

        protobuf::assign_repeated(
            native.mapping_keys,
            proto->mutable_mapping_keys());
    }

    void decode(const cc::platform::pubsub::protobuf::ReplayPolicy& proto,
                cc::platform::pubsub::ReplayPolicy* native)
    {
        native->replay_latest = proto.replay_latest();

        protobuf::assign_to_vector(
            proto.mapping_keys(),
            &native->mapping_keys);
    }

    //--------------------------------------------------------------------------
    // ReplayPolicyMap

    void encode(const cc::platform::pubsub::ReplayPolicyMap& native,
                cc::platform::pubsub::protobuf::ReplayPolicyMap* proto)
    {
        auto &map = *proto->mutable_map();
        for (const auto& [key, value] : native)
        {
            encode(value, &map[key]);
        }
    }

    void decode(const cc::platform::pubsub::protobuf::ReplayPolicyMap& proto,
                cc::platform::pubsub::ReplayPolicyMap* native)
    {
        for (const auto& [key, value] : proto.map())
        {
            decode(value, &(*native)[key]);
        }
    }

}  // namespace cc::protobuf
