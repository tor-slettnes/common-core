// -*- c++ -*-
//==============================================================================
/// @file test-demo-native.c++
/// @brief C++ demo - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "test-demo-fixture.h++"
#include "demo-native.h++"

namespace demo
{
    class NativeTest : public DemoTest
    {
    protected:
        NativeTest()
            : DemoTest(demo::NativeImpl::create_shared("DemoTest"))
        {
        }
    };

    TEST_F(NativeTest, GotProvider)
    {
        this->test_got_provider();
    }

    TEST_F(NativeTest, GotCurrentTime)
    {
        this->test_got_current_time();
    }

    TEST_F(NativeTest, Hello)
    {
        this->test_say_hello();
    }

    TEST_F(NativeTest, EncodeDecodeGreeting)
    {
        this->test_encode_decode_greeting();
    }

    TEST_F(NativeTest, EncodeDecodeTimeData)
    {
        this->test_encode_decode_timedata();
    }

}  // namespace demo
