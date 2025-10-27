// -*- c++ -*-
//==============================================================================
/// @file protobuf-demo-types.h++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "cc/demo/protobuf/demo_types.pb.h"
#include <ctime>

namespace protobuf
{
    //==========================================================================
    // Greeting
    void encode(const demo::Greeting &greeting, cc::demo::protobuf::Greeting *msg);
    void decode(const cc::demo::protobuf::Greeting &msg, demo::Greeting *greeting);

    //==========================================================================
    // TimeData
    void encode(const demo::TimeData &timedata, cc::demo::protobuf::TimeData *msg);
    void decode(const cc::demo::protobuf::TimeData &msg, demo::TimeData *timedata);
  // namespace protobuf
}
