/// -*- c++ -*-
//==============================================================================
/// @file protobuf-datetime-types.h++
/// @brief Encode/decode routines for types from `datetime.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "datetime.pb.h"  // generated from `datetime.proto`
#include "chrono/date-time.h++"

namespace protobuf
{
    //==========================================================================
    // TimeUnit

    void encode(core::dt::TimeUnit native, cc::datetime::TimeUnit *proto);
    void decode(cc::datetime::TimeUnit proto, core::dt::TimeUnit *native);

    //==========================================================================
    // DateTimeInterval

    void encode(const core::dt::DateTimeInterval &native, cc::datetime::Interval *proto);
    void decode(const cc::datetime::Interval &proto, core::dt::DateTimeInterval *native);

    //==========================================================================
    // TimeStruct

    void encode(const std::tm &tm, cc::datetime::TimeStruct *msg);
    void decode(const cc::datetime::TimeStruct &msg, std::tm *tm);

}  // namespace protobuf
