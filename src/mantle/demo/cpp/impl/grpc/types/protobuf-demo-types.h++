// -*- c++ -*-
//==============================================================================
/// @file protobuf-demo-types.h++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-types.h++"
#include "demo.pb.h"
#include <ctime>

namespace protobuf
{
    //==========================================================================
    // Greeting
    void encode(const demo::Greeting &greeting, CC::Demo::Greeting *msg);
    void decode(const CC::Demo::Greeting &msg, demo::Greeting *greeting);

    //==========================================================================
    // TimeData
    void encode(const demo::TimeData &timedata, CC::Demo::TimeData *msg);
    void decode(const CC::Demo::TimeData &msg, demo::TimeData *timedata);

    //==========================================================================
    // TimeStruct
    void encode(const std::tm &tm, CC::Demo::TimeStruct *msg);
    void decode(const CC::Demo::TimeStruct &msg, std::tm *tm);
}  // namespace core::protobuf
