/// -*- c++ -*-
//==============================================================================
/// @file protobuf-datetime-types.h++
/// @brief Encode/decode routines for types from `datetime.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/protobuf/datetime/datetime.pb.h"  // generated from `datetime.proto`
#include "chrono/date-time.h++"

namespace protobuf
{
    //==========================================================================
    // TimeUnit

    void encode(core::dt::TimeUnit native, cc::protobuf::datetime::TimeUnit *proto);
    void decode(cc::protobuf::datetime::TimeUnit proto, core::dt::TimeUnit *native);

    //==========================================================================
    // DateTimeInterval

    void encode(const core::dt::DateTimeInterval &native, cc::protobuf::datetime::Interval *proto);
    void decode(const cc::protobuf::datetime::Interval &proto, core::dt::DateTimeInterval *native);

    //==========================================================================
    // TimeStruct

    void encode(const std::tm &tm, cc::protobuf::datetime::TimeStruct *msg);
    void decode(const cc::protobuf::datetime::TimeStruct &msg, std::tm *tm);

}  // namespace protobuf
