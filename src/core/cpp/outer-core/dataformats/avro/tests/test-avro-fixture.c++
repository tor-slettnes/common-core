// -*- c++ -*-
//==============================================================================
/// @file test-avro-fixture.c++
/// @brief C++ avro - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "test-avro-fixture.h++"
#include "avro-protobufschema.h++"

namespace core::avro
{
        void SetUp() override;
        void TearDown() override;

        void test_convert_event_to_json_schema();
    };

} // namespace core::avro
