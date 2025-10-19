// -*- c++ -*-
//==============================================================================
/// @file protobuf-demo-types.h++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "cc/protobuf/demo/demo.pb.h"
#include <ctime>

namespace protobuf
{
    //==========================================================================
    // Greeting
    void encode(const demo::Greeting &greeting, cc::demo::Greeting *msg);
    void decode(const cc::demo::Greeting &msg, demo::Greeting *greeting);

    //==========================================================================
    // TimeData
    void encode(const demo::TimeData &timedata, cc::demo::TimeData *msg);
    void decode(const cc::demo::TimeData &msg, demo::TimeData *timedata);
  // namespace protobuf
}
