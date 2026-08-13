// -*- c++ -*-
//==============================================================================
/// @file protobuf-demo-types.h++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "cc/demo/protobuf/demo_types.pb.h"
#include <ctime>

namespace cc::protobuf
{
    //--------------------------------------------------------------------------
    // Greeting

    void encode(const cc::demo::Greeting &greeting,
                cc::demo::protobuf::Greeting *msg);

    void decode(const cc::demo::protobuf::Greeting &msg,
                cc::demo::Greeting *greeting);

    //--------------------------------------------------------------------------
    // TimeData

    void encode(const cc::demo::TimeData &timedata,
                cc::demo::protobuf::TimeData *msg);

    void decode(const cc::demo::protobuf::TimeData &msg,
                cc::demo::TimeData *timedata);
    // namespace cc::protobuf
}  // namespace cc::protobuf
