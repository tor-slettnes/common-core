// -*- c++ -*-
//==============================================================================
/// @file test-string-expand.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/expand.h++"
#include "types/value.h++"

#include <gtest/gtest.h>

namespace core::str
{
    TEST(StringTest, Expand)
    {
        std::string original{
            "one={one}, "
            "two={two}, "
            "three={three}, "
            "four={four}, "
            "five={five}"};

        std::unordered_map<std::string, types::Value> replacements = {
            {"one", true},
            {"two", 2},
            {"three", 3.141592653589793238},
            {"four", "IV"},
            {"five", std::chrono::hours(5)},
        };

        std::string expanded = expand(original, replacements);

        std::string expected{
            "one=true, "
            "two=2, "
            "three=3.14159, "
            "four=IV, "
            "five=5 hr"};

        EXPECT_EQ(expanded, expected);
    }
}  // namespace core::str
