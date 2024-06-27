// -*- c++ -*-
//==============================================================================
/// @file test-demo-fixture.h++
/// @brief C++ demo - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-provider.h++"

#include <gtest/gtest.h>

namespace demo
{
    class DemoTest : public testing::Test
    {
    protected:
        DemoTest(std::shared_ptr<demo::API> demo_provider);

        void SetUp() override;
        void TearDown() override;

        void test_got_provider();
        void test_got_current_time();
        void test_say_hello();
        void test_encode_decode_greeting();
        void test_encode_decode_timedata();

    protected:
        std::shared_ptr<demo::API> demo_provider;
        demo::Greeting test_greeting;
    };

}  // namespace demo
