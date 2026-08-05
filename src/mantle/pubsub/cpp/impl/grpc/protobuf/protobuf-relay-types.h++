// -*- c++ -*-
//==============================================================================
/// @file protobuf-relay-types.h++
/// @brief conversions to/from Protocol Buffer messages in "pubsub_types.proto"
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "cc/platform/pubsub/protobuf/pubsub_types.pb.h"

namespace cc::protobuf
{
    //--------------------------------------------------------------------------
    // KeyPath
    void encode(const cc::platform::pubsub::KeyPath &native,
                cc::platform::pubsub::protobuf::KeyPath *proto);

    void decode(const cc::platform::pubsub::protobuf::KeyPath &proto,
                cc::platform::pubsub::KeyPath *native);

    //--------------------------------------------------------------------------
    // ReplayPolicy
    void encode(const cc::platform::pubsub::ReplayPolicy &native,
                cc::platform::pubsub::protobuf::ReplayPolicy *proto);

    void decode(const cc::platform::pubsub::protobuf::ReplayPolicy &proto,
                cc::platform::pubsub::ReplayPolicy *native);

    //--------------------------------------------------------------------------
    // ReplayPolicyMap

    void encode(const cc::platform::pubsub::ReplayPolicyMap &native,
                cc::platform::pubsub::protobuf::ReplayPolicyMap *proto);

    void decode(const cc::platform::pubsub::protobuf::ReplayPolicyMap &proto,
                cc::platform::pubsub::ReplayPolicyMap *native);

}


