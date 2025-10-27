// -*- c++ -*-
//==============================================================================
/// @file protobuf-demo-types.c++
/// @brief conversions to/from Protocol Buffer messages for Demo application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-demo-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-datetime-types.h++"

namespace protobuf
{
    //==========================================================================
    // Greeting

    void encode(const demo::Greeting &greeting, cc::demo::protobuf::Greeting *msg)
    {
        msg->set_text(greeting.text);
        msg->set_identity(greeting.identity);
        msg->set_implementation(greeting.implementation);
        encode(greeting.birth, msg->mutable_birth());
        encode(greeting.data, msg->mutable_data());
    }

    void decode(const cc::demo::protobuf::Greeting &msg, demo::Greeting *greeting)
    {
        greeting->text = msg.text();
        greeting->identity = msg.identity();
        greeting->implementation = msg.implementation();
        decode(msg.birth(), &greeting->birth);
        decode(msg.data(), &greeting->data);
    }

    //==========================================================================
    // TimeData

    void encode(const demo::TimeData &timedata, cc::demo::protobuf::TimeData *msg)
    {
        encode(timedata.timepoint, msg->mutable_timestamp());
        encode(timedata.localtime, msg->mutable_local_time());
        encode(timedata.utctime, msg->mutable_utc_time());
    }

    void decode(const cc::demo::protobuf::TimeData &msg, demo::TimeData *timedata)
    {
        decode(msg.timestamp(), &timedata->timepoint);
        decode(msg.local_time(), &timedata->localtime);
        decode(msg.utc_time(), &timedata->utctime);
    }
}  // namespace protobuf
