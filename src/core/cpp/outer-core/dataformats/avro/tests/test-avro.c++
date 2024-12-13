// -*- c++ -*-
//==============================================================================
/// @file test-avro.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-simplevalue.h++"

#include <gtest/gtest.h>


namespace core::avro
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
}
