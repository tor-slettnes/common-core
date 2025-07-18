// -*- c++ -*-
//==============================================================================
/// @file test-avro.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-simplevalue.h++"
#include "avro-compoundvalue.h++"
#include "avro-protobufschema.h++"
#include "avro-protobufvalue.h++"
#include "protobuf-event-types.h++"
#include "status.pb.h"
#include "parsers/json/writer.h++"
#include "parsers/json/reader.h++"

#include <gtest/gtest.h>
#include <fstream>

namespace avro
{
    TEST(AvroTest, SimpleStringValue)
    {
        std::string orig_string = "sample avro input string";
        SimpleValue sv(orig_string);
        std::optional<std::string> new_string = sv.get_string();

        EXPECT_TRUE(new_string.has_value());
        EXPECT_EQ(orig_string.size(), new_string->size());
        EXPECT_EQ(orig_string, *new_string);
        EXPECT_EQ(sv.as_value().type(), core::types::ValueType::STRING);

        avro_schema_t schema = avro_value_get_schema(sv.c_value());
        EXPECT_TRUE(is_avro_string(schema));

        std::optional<double> new_double = sv.get_double();
        EXPECT_FALSE(new_double.has_value());
    }

    TEST(AvroTest, ProtoBufEventToJsonSchema)
    {
        const SchemaWrapper &wrapper = schema_from_proto(cc::status::Error::GetDescriptor());
        // std::string json = wrapper.as_json();
        std::string json = core::json::writer.encoded(wrapper, true);
        auto of1 = std::ofstream("event.json");
        of1.write(json.data(), json.size());
        EXPECT_TRUE(of1.tellp() > 0);
        of1.close();
    }

    TEST(AvroTest, VariantToAvro)
    {
        auto context = std::make_shared<BuilderContext>();
        CompoundValue compound{VariantSchema(context)};

        core::types::KeyValueMap kvmap = {
            {"my_bool", true},
            {"my_int", 42},
            {"my_real", 3.141592653589793238},
            {"my_string", "Some text here"},
            {"my_bytes", core::types::ByteVector::from_string("some bytes here")},
            {"my_map", core::types::KeyValueMap({{"one", 1}, {"two", "II"}, {"three", 3.14}})},
            {"my_list", core::types::ValueList({true, 2, 3.141592653589793238, "IV"})},
            {"my_timestamp", core::dt::Clock::now()},
            {"my_5_seconds", std::chrono::seconds(5)},
        };

        compound.set_variant(compound.c_value(), kvmap);
        std::string json = compound.as_json(true);

        auto of = std::ofstream("variant.json");
        of.write(json.data(), json.size());
        EXPECT_TRUE(of.tellp() > 0);
        of.close();
    }

    TEST(AvroTest, ProtoBufToAvro)
    {
        core::dt::TimePoint tp = core::dt::Clock::now();
        double pi = 3.141592653589793238;
        std::string text("Arbitrary Event Text");
        std::string origin("test case");
        uint code = 42;
        std::string symbol = "LifeUniverseEverything";

        core::dt::Duration dur_value = std::chrono::microseconds(1000001);
        std::string string_value = "II";
        bool bool_value = true;
        core::dt::TimePoint tp_value = tp - std::chrono::hours(24);
        double real_value = pi;

        core::status::Error event(
            text,                               // text
            core::status::Domain::APPLICATION,  // domain
            origin,                             // origin
            code,                               // code
            symbol,                             // symbol
            core::status::Level::NOTICE,        // level
            tp,                                 // timepoint,
            {
                {"my_bool", bool_value},
                {"my_string", string_value},
                {"my_real", real_value},
                {"my_tp", tp_value},
                {"my_duration", dur_value},
            });

        cc::status::Error msg;
        protobuf::encode(event, &msg);
        avro::ProtoBufValue avro_wrapper(msg);

        // EXPECT_EQ(avro_wrapper.as_value().get("attributes").get("duration").as_bytevector().size(), 12);

        std::string json_text = avro_wrapper.as_json();

        std::ofstream of("avro-event.json");
        of.write(json_text.data(), json_text.size());

        core::types::Value readback = core::json::reader.decoded(json_text);

        EXPECT_EQ(readback.get("text").as_string(), text);
        EXPECT_EQ(readback.get("domain").as_string(), "APPLICATION");
        EXPECT_EQ(readback.get("origin").as_string(), origin);
        EXPECT_EQ(readback.get("code").as_uint(), code);
        EXPECT_EQ(readback.get("symbol").as_string(), symbol);
        EXPECT_EQ(readback.get("timestamp").as_sint64(), core::dt::to_milliseconds(tp));

        core::types::KeyValueMap attributes = readback.get("attributes").as_kvmap();

        EXPECT_EQ(attributes.get("my_bool").get("value").get("boolean").as_bool(),
                  bool_value);

        core::types::Value observed_ms = attributes.get("my_duration").get("value").get("long");
        auto expected_ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur_value);

        EXPECT_EQ(observed_ms.as_sint(), expected_ms.count());
    }
}  // namespace avro
