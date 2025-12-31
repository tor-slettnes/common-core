// -*- c++ -*-
//==============================================================================
/// @file test-string-misc.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes
//==============================================================================

#include "types/value.h++"

#include <gtest/gtest.h>

namespace core::types
{
    class DerivedValue : public Value
    {
    public:
        using Value::Value;
    };

    TEST(Variant, ConstructValue)
    {
        EXPECT_EQ(Value(true).as_bool(), true);
        EXPECT_EQ(Value(42).as_uint(), 42);
        EXPECT_EQ(Value(3.141592653589793238).as_double(), 3.141592653589793238);
        EXPECT_EQ(Value("Some string").as_string(), "Some string");
    }

    TEST(Variant, ConstructList)
    {
        types::ValueList vlist = {
            false,
            1,
            "II",
            3.141592653589793238,
            std::complex(1.1, 2.2),
            dt::Clock::now(),
            std::chrono::seconds(5),
        };

        types::KeyValueMap kvmap = {
            {"first", 1},
            {"second", 2},
            {"third", vlist},
        };

        types::KeyValueMap container = {
            {"vlist", vlist},
            {"kvmap", kvmap},
        };

        EXPECT_EQ(Value(vlist).as_valuelist(), vlist);
        EXPECT_EQ(Value(kvmap).as_kvmap(), kvmap);
        EXPECT_EQ(container.at("vlist").as_valuelist(), vlist);
        EXPECT_EQ(container.at("kvmap").as_kvmap(), kvmap);

        const types::KeyValueMap container2 = {
            {"vlist", std::move(vlist)},
            {"kvmap", std::move(kvmap)},
        };

        EXPECT_EQ(container, container2);

        Value value2 = container2;
        EXPECT_EQ(value2.as_kvmap(), container);

        DerivedValue value3(value2);
        EXPECT_EQ(value2, value3);
    }

    // TEST(StringTest, WideString)
    // {
    //     std::string latin1_no("abcdefghijklmnopqrstuvwxyz\xe6\xf8\xe5");
    //     std::string utf8_no("abcdefghijklmnopqrstuvwxyz\xc3\xa6\xc3\xb8\xc3\xa5");

    //     std::wstring widened_latin1_no(to_wstring(latin1_no, LATIN1));
    //     std::wstring widened_utf8_no(to_wstring(utf8_no, UTF8));

    //     EXPECT_THROW(to_wstring(latin1_no, UTF8), std::invalid_argument);

    //     EXPECT_EQ(widened_latin1_no.size(), 29);
    //     EXPECT_EQ(widened_utf8_no.size(), 29);
    //     EXPECT_EQ(widened_latin1_no, widened_utf8_no);

    //     std::string narrowed_latin1_no(from_wstring(widened_latin1_no, LATIN1));
    //     std::string narrowed_utf8_no(from_wstring(widened_utf8_no, UTF8));

    //     EXPECT_EQ(narrowed_latin1_no, latin1_no);
    //     EXPECT_EQ(narrowed_utf8_no, utf8_no);
    // }

    // TEST(StringTest, ObfuscateWide)
    // {
    //     std::string input{"My#Top*Secret%Code"};
    //     std::string result = obfuscated(input);
    //     std::wstring wide_result = to_wstring(result, UTF8);

    //     EXPECT_EQ(input.size(), wide_result.size());
    //     EXPECT_EQ(wide_result.front(), wide_result.back());
    // }

} // namespace core::types
