// -*- c++ -*-
//==============================================================================
/// @file test-demo-native.c++
/// @brief C++ demo - test routines
/// @author Tor Slettnes
//==============================================================================

#include "test-demo-fixture.h++"
#include "demo-native.h++"

namespace demo
{
    class DemoNativeTest : public DemoTest
    {
    protected:
        DemoNativeTest()
            : DemoTest(demo::NativeImpl::create_shared("DemoTest"))
        {
        }
    };

    TEST_F(DemoNativeTest, GotProvider)
    {
        this->test_got_provider();
    }

    TEST_F(DemoNativeTest, GotCurrentTime)
    {
        this->test_got_current_time();
    }

    TEST_F(DemoNativeTest, Hello)
    {
        this->test_say_hello();
    }

    TEST_F(DemoNativeTest, EncodeDecodeGreeting)
    {
        this->test_encode_decode_greeting();
    }

    TEST_F(DemoNativeTest, EncodeDecodeTimeData)
    {
        this->test_encode_decode_timedata();
    }

}  // namespace demo
