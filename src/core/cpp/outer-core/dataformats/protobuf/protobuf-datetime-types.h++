/// -*- c++ -*-
//==============================================================================
/// @file protobuf-datetime-types.h++
/// @brief Encode/decode routines for types from `datetime.proto`
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/datetime/datetime.pb.h"  // generated from `datetime.proto`
#include "chrono/date-time.h++"

namespace cc::protobuf
{
    //==========================================================================
    // TimeUnit

    void encode(core::dt::TimeUnit native, datetime::TimeUnit* proto);
    void decode(datetime::TimeUnit proto, core::dt::TimeUnit* native);

    //==========================================================================
    // DateTimeInterval

    void encode(const core::dt::DateTimeInterval& native, datetime::Interval* proto);
    void decode(const datetime::Interval& proto, core::dt::DateTimeInterval* native);

    //==========================================================================
    // TimeStruct

    void encode(const std::tm& tm, datetime::TimeStruct* msg);
    void decode(const datetime::TimeStruct& msg, std::tm* tm);

}  // namespace cc::protobuf
