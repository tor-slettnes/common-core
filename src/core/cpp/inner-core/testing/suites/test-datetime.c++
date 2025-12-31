// -*- c++ -*-
//==============================================================================
/// @file test-datetime.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes
//==============================================================================

#include "chrono/date-time.h++"

#include <gtest/gtest.h>

namespace core::str
{
    TEST(DateTimeTest, LastAlignedInterval)
    {
        dt::TimePoint tp = dt::Clock::now();
    }

}  // namespace core::str
