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

    TEST(AvroTest, VariantToAvro)
    {
        CompoundValue compound{VariantSchema()};

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
        auto of = std::ofstream("variant.json");
        std::string json = compound.as_json(true);
        of.write(json.data(), json.size());
        EXPECT_TRUE(of.tellp() > 0);
        of.close();
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
                {"now", core::dt::Clock::now()},
                {"duration", std::chrono::seconds(5)},
            });

        cc::status::Event msg;
        protobuf::encode(event, &msg);
        avro::ProtoBufValue avro_wrapper(msg);

        // EXPECT_EQ(avro_wrapper.as_value().get("attributes").get("duration").as_bytevector().size(), 12);

        std::string text = avro_wrapper.as_json();
        std::ofstream of("avro-event.json");
        of.write(text.data(), text.size());
        EXPECT_FALSE(text.empty());
    }
}  // namespace avro
