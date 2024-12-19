// -*- c++ -*-
//==============================================================================
/// @file test-avro-fixture.h++
/// @brief C++ avro - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <gtest/gtest.h>

namespace core::avro
{
    class AvroTest : public testing::Test
    {
    protected:
        void SetUp() override;
        void TearDown() override;

        void test_convert_event_to_json_schema();
    };

} // namespace core::avro
