/// -*- c++ -*-
//==============================================================================
/// @file protobuf-datetime-types.c++
/// @brief Encode/decode routines for types from `datetime.proto`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-datetime-types.h++"

namespace protobuf
{
    //==========================================================================
    // TimeUnit

    void encode(core::dt::TimeUnit native, cc::datetime::TimeUnit *proto)
    {
        *proto = static_cast<cc::datetime::TimeUnit>(native);
    }

    void decode(cc::datetime::TimeUnit proto, core::dt::TimeUnit *native)
    {
        *native = static_cast<core::dt::TimeUnit>(proto);
    }

    //==========================================================================
    // DateTimeInterval

    void encode(const core::dt::DateTimeInterval &native, cc::datetime::Interval *proto)
    {
        proto->set_unit(static_cast<cc::datetime::TimeUnit>(native.unit));
        proto->set_count(native.count);
    }

    void decode(const cc::datetime::Interval &proto, core::dt::DateTimeInterval *native)
    {
        native->unit = static_cast<core::dt::TimeUnit>(proto.unit());
        native->count = proto.count();
    }

    //==========================================================================
    // TimeStruct

    void encode(const std::tm &tm, cc::datetime::TimeStruct *msg)
    {
        msg->set_year(tm.tm_year + core::dt::TM_YEAR_OFFSET);
        msg->set_month(tm.tm_mon + core::dt::TM_MONTH_OFFSET);
        msg->set_day(tm.tm_mday + core::dt::TM_DAY_OFFSET);
        msg->set_hour(tm.tm_hour);
        msg->set_minute(tm.tm_min);
        msg->set_second(tm.tm_sec);
        msg->set_weekday(static_cast<cc::datetime::Weekday>(tm.tm_wday + core::dt::TM_WEEKDAY_OFFSET));
        msg->set_year_day(tm.tm_yday + core::dt::TM_YEARDAY_OFFSET);
        msg->set_is_dst(tm.tm_isdst > 0);
    }

    void decode(const cc::datetime::TimeStruct &msg, std::tm *tm)
    {
        *tm = {
            .tm_sec = static_cast<int>(msg.second()),
            .tm_min = static_cast<int>(msg.minute()),
            .tm_hour = static_cast<int>(msg.hour()),
            .tm_mday = static_cast<int>(msg.day()) - core::dt::TM_DAY_OFFSET,
            .tm_mon = static_cast<int>(msg.month()) - core::dt::TM_MONTH_OFFSET,
            .tm_year = static_cast<int>(msg.year()) - core::dt::TM_YEAR_OFFSET,
            .tm_wday = static_cast<int>(msg.weekday()) - core::dt::TM_WEEKDAY_OFFSET,
            .tm_yday = static_cast<int>(msg.year_day()) - core::dt::TM_YEARDAY_OFFSET,
            .tm_isdst = static_cast<int>(msg.is_dst()),
        };
    }
}  // namespace protobuf
