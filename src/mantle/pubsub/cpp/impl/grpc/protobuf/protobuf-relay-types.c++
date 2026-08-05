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
    // KeyPath
    void encode(const cc::platform::pubsub::KeyPath &native,
                cc::platform::pubsub::protobuf::KeyPath *proto)
    {
        protobuf::assign_repeated(native, proto->mutable_elements());
    }

    void decode(const cc::platform::pubsub::protobuf::KeyPath &proto,
                cc::platform::pubsub::KeyPath *native)
    {
        protobuf::assign_to_vector(proto.elements(), native);
    }

    //--------------------------------------------------------------------------
    // ReplayPolicy
    void encode(const cc::platform::pubsub::ReplayPolicy& native,
                cc::platform::pubsub::protobuf::ReplayPolicy* proto)
    {
        proto->set_replay_latest(native.replay_latest);
        encode_vector(native.key_paths, proto->mutable_key_paths());
    }

    void decode(const cc::platform::pubsub::protobuf::ReplayPolicy& proto,
                cc::platform::pubsub::ReplayPolicy* native)
    {
        native->replay_latest = proto.replay_latest();
        decode_to_vector(proto.key_paths(), &native->key_paths);
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
