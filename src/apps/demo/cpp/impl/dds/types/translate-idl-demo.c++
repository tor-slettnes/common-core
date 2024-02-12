/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-demo.c++
/// @brief Encode/decode routines for Demo IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-demo.h++"  // IDL types, generated from `demo-types.idl`
#include "translate-idl-inline.h++"
#include "chrono/date-time.h++"

namespace idl
{
    //==========================================================================
    // Greeting
    void encode(const demo::Greeting &native,
                CC::Demo::Greeting *idl)
    {
        idl->text(native.text);
        idl->identity(native.identity);
        idl->implementation(native.implementation);
        encode(native.birth, &idl->birth());
        encode(native.data, &idl->data());
    }

    void decode(const CC::Demo::Greeting &idl,
                demo::Greeting *native)
    {
        native->text = idl.text();
        native->identity = idl.identity();
        native->implementation = idl.implementation();
        decode(idl.birth(), &native->birth);
        decode(idl.data(), &native->data);
    }

    //==========================================================================
    // TimeData

    void encode(const demo::TimeData &native,
                CC::Demo::TimeData *idl)
    {
        encode(native.timepoint, &idl->timestamp());
        encode(native.localtime, &idl->local_time());
        encode(native.utctime, &idl->utc_time());
    }

    void decode(const CC::Demo::TimeData &idl,
                demo::TimeData *native)
    {
        decode(idl.timestamp(), &native->timepoint);
        decode(idl.local_time(), &native->localtime);
        decode(idl.utc_time(), &native->utctime);
    }

    //==========================================================================
    // TimeStruct

    void encode(const std::tm &native,
                CC::Demo::TimeStruct *idl)
    {
        idl->year(native.tm_year + shared::dt::TM_YEAR_OFFSET);
        idl->month(native.tm_mon + shared::dt::TM_MONTH_OFFSET);
        idl->day(native.tm_mday + shared::dt::TM_DAY_OFFSET);
        idl->hour(native.tm_hour);
        idl->minute(native.tm_min);
        idl->second(native.tm_sec);
        idl->weekday(static_cast<CC::Demo::Weekday>(native.tm_wday + shared::dt::TM_WEEKDAY_OFFSET));
        idl->year_day(native.tm_yday + shared::dt::TM_YEARDAY_OFFSET);

        if (native.tm_isdst >= 0)
        {
            idl->is_dst(native.tm_isdst > 0);
        }
    }

    void decode(const CC::Demo::TimeStruct &idl,
                std::tm *native)
    {
        native->tm_sec = static_cast<int>(idl.second());
        native->tm_min = static_cast<int>(idl.minute());
        native->tm_hour = static_cast<int>(idl.hour());
        native->tm_mday = static_cast<int>(idl.day()) - shared::dt::TM_DAY_OFFSET;
        native->tm_mon = static_cast<int>(idl.month()) - shared::dt::TM_MONTH_OFFSET;
        native->tm_year = static_cast<int>(idl.year()) - shared::dt::TM_YEAR_OFFSET;
        native->tm_mday = static_cast<int>(idl.weekday()) - shared::dt::TM_WEEKDAY_OFFSET;
        native->tm_yday = static_cast<int>(idl.year_day()) - shared::dt::TM_YEARDAY_OFFSET;
        native->tm_isdst = static_cast<int>(idl.is_dst().has_value() ? idl.is_dst().value() : -1);
    }

}  // namespace idl
