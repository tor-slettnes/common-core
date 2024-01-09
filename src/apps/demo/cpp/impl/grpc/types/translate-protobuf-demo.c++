// -*- c++ -*-
//==============================================================================
/// @file translate-protobuf-demo.c++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-protobuf-demo.h++"
#include "translate-protobuf-google.h++"
#include "translate-protobuf-variant.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Greeting

    void encode(const demo::Greeting &greeting, CC::Demo::Greeting *msg)
    {
        msg->set_text(greeting.text);
        msg->set_identity(greeting.identity);
        msg->set_implementation(greeting.implementation);
        encode(greeting.birth, msg->mutable_birth());
        encode(greeting.data, msg->mutable_data());
    }

    void decode(const CC::Demo::Greeting &msg, demo::Greeting *greeting)
    {
        greeting->text = msg.text();
        greeting->identity = msg.identity();
        greeting->implementation = msg.implementation();
        decode(msg.birth(), &greeting->birth);
        decode(msg.data(), &greeting->data);
    }

    //==========================================================================
    // TimeData

    void encode(const demo::TimeData &timedata, CC::Demo::TimeData *msg)
    {
        encode(timedata.timepoint, msg->mutable_timestamp());
        encode(timedata.localtime, msg->mutable_local_time());
        encode(timedata.utctime, msg->mutable_utc_time());
    }

    void decode(const CC::Demo::TimeData &msg, demo::TimeData *timedata)
    {
        decode(msg.timestamp(), &timedata->timepoint);
        decode(msg.local_time(), &timedata->localtime);
        decode(msg.utc_time(), &timedata->utctime);
    }

    //==========================================================================
    // TimeStruct

    void encode(const std::tm &tm, CC::Demo::TimeStruct *msg)
    {
        msg->set_year(tm.tm_year + dt::TM_YEAR_OFFSET);
        msg->set_month(tm.tm_mon + dt::TM_MONTH_OFFSET);
        msg->set_day(tm.tm_mday + dt::TM_DAY_OFFSET);
        msg->set_hour(tm.tm_hour);
        msg->set_minute(tm.tm_min);
        msg->set_second(tm.tm_sec);
        msg->set_weekday(static_cast<CC::Demo::Weekday>(tm.tm_wday + dt::TM_WEEKDAY_OFFSET));
        msg->set_year_day(tm.tm_yday + dt::TM_YEARDAY_OFFSET);
        msg->set_is_dst(tm.tm_isdst > 0);
    }

    void decode(const CC::Demo::TimeStruct &msg, std::tm *tm)
    {
        *tm = {
            .tm_sec = static_cast<int>(msg.second()),
            .tm_min = static_cast<int>(msg.minute()),
            .tm_hour = static_cast<int>(msg.hour()),
            .tm_mday = static_cast<int>(msg.day()) - dt::TM_DAY_OFFSET,
            .tm_mon = static_cast<int>(msg.month()) - dt::TM_MONTH_OFFSET,
            .tm_year = static_cast<int>(msg.year()) - dt::TM_YEAR_OFFSET,
            .tm_wday = static_cast<int>(msg.weekday()) - dt::TM_WEEKDAY_OFFSET,
            .tm_yday = static_cast<int>(msg.year_day()) - dt::TM_YEARDAY_OFFSET,
            .tm_isdst = static_cast<int>(msg.is_dst()),
        };
    }
}  // namespace cc::protobuf
