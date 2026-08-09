// -*- c++ -*-
//==============================================================================
/// @file test-protobuf.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-message-decoder.h++"
#include "protobuf-message-encoder.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "chrono/date-time.h++"
#include "status/error.h++"

#include "cc/protobuf/status/status.pb.h"

#include <gtest/gtest.h>

namespace cc::protobuf
{
    const core::status::Error sample_error(
        "test error text",                  // text
        core::status::Domain::APPLICATION,  // domain
        "test origin",                      // origin
        42,                                 // code
        "test symbol",                      // symbol
        core::status::Level::ERROR,         // level
        core::dt::Clock::now(),             // timepoint
        {
            // attributes
            {"one", true},
            {"two", 2},
            {"three", 3.141592653589793238},
            {"four", "IV"},
            {"five", -5},
            {"six", {}},
            {"seven", core::types::ValueList{
                          false,
                          1,
                          2.718281828459045,
                          "III",
                      }},
        });

    TEST(ProtoBufTest, NativeToProto)
    {
        status::Error proto_error;
        encode(sample_error, &proto_error);

        EXPECT_EQ(proto_error.text(), sample_error.text());
        EXPECT_EQ(static_cast<int>(proto_error.domain()),
                  static_cast<int>(sample_error.domain()));

        EXPECT_EQ(proto_error.origin(), sample_error.origin());
        EXPECT_EQ(static_cast<int>(proto_error.level()),
                  static_cast<int>(sample_error.level()));

        EXPECT_EQ(proto_error.code(), sample_error.code());
        EXPECT_EQ(proto_error.symbol(), sample_error.symbol());

        EXPECT_EQ(decoded<core::dt::TimePoint>(proto_error.timestamp()),
                  sample_error.timepoint());

        EXPECT_EQ(decoded<core::types::KeyValueMap>(proto_error.attributes()),
                  sample_error.attributes());
    }

    TEST(ProtoBufTest, NativeToProtoAndBack)
    {
        status::Error proto_error;
        encode(sample_error, &proto_error);

        core::status::Error decoded_error;
        decode(proto_error, &decoded_error);

        EXPECT_EQ(decoded_error, sample_error);
    }

    TEST(ProtoBufTest, NativeToProtoThenVariant)
    {
        status::Error proto_error;
        encode(sample_error, &proto_error);

        core::types::Value value = to_value(proto_error);
        EXPECT_TRUE(value.is_kvmap());

        EXPECT_EQ(value.get("text").get_string(), sample_error.text());
        EXPECT_EQ(value.get("domain").get_string(),
                  core::status::domain_names.to_string(sample_error.domain()));

        EXPECT_EQ(value.get("origin").get_string(), sample_error.origin());
        EXPECT_EQ(value.get("level").get_string(),
                  core::status::level_names.to_string(sample_error.level()));

        EXPECT_EQ(value.get("code").as_sint64(),
                  sample_error.code());

        EXPECT_EQ(value.get("symbol").get_string(),
                  sample_error.symbol());

        EXPECT_EQ(value.get("timestamp").as_timepoint(),
                  sample_error.timepoint());

        EXPECT_EQ(value.get("attributes").get_kvmap(),
                  sample_error.attributes());
    }

    TEST(ProtoBufTest, ReconstructProtoFromVariant)
    {
        status::Error proto_error;
        encode(sample_error, &proto_error);

        core::types::Value deconstructed = to_value(proto_error);

        auto reconstructed_proto = to_message<status::Error>(deconstructed);
        EXPECT_EQ(reconstructed_proto, proto_error);
    }

    TEST(ProtoBufTest, NativeToProtoThenVariantAndBack)
    {
        status::Error proto_error;
        encode(sample_error, &proto_error);

        core::types::Value deconstructed = to_value(proto_error);

        auto reconstructed_proto = to_message<status::Error>(deconstructed);
        auto reconstructed_native = decoded<core::status::Error>(reconstructed_proto);
        EXPECT_EQ(reconstructed_native, sample_error);
    }
}  // namespace cc::protobuf
