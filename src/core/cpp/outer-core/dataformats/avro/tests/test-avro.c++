// -*- c++ -*-
//==============================================================================
/// @file test-avro.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-simplevalue.h++"
#include "avro-protobufschema.h++"
#include "avro-protobufvalue.h++"
#include "protobuf-event-types.h++"
#include "status.pb.h"
#include "parsers/json/writer.h++"

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
        SchemaWrapper wrapper = schema_from_proto(cc::status::Event::GetDescriptor());
        // std::string json = wrapper.as_json();
        std::string json = core::json::writer.encoded(wrapper, true);
        auto of1 = std::ofstream("event.json");
        of1.write(json.data(), json.size());
        EXPECT_TRUE(of1.tellp() > 0);
        of1.close();
    }

    TEST(AvroTest, ProtoBufToAvro)
    {
        core::status::Event event(
            "Arbitrary Event Text",             // text
            core::status::Domain::APPLICATION,  // domain
            "test case",                        // origin
            42,                                 // code
            "LifeUniverseEverything",           // symbol
            core::status::Level::NOTICE,        // level
            core::dt::Clock::now(),             // timepoint,
            {
                {"key1", true},
                {"key2", "II"},
                {"key3", 3.141592653589793238},
            });

        cc::status::Event msg;
        protobuf::encode(event, &msg);
        avro::ProtoBufValue avro_wrapper(msg);

        std::string text = avro_wrapper.as_json();
        std::ofstream of("avro-event.json");
        of.write(text.data(), text.size());
        EXPECT_FALSE(text.empty());
    }
}  // namespace avro
