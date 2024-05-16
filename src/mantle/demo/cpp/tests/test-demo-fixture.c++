// -*- c++ -*-
//==============================================================================
/// @file test-demo-fixture.c++
/// @brief C++ demo - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "test-demo-fixture.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-inline.h++"
#include "chrono/date-time.h++"

namespace demo
{
    DemoTest::DemoTest(std::shared_ptr<demo::API> demo_provider)
        : demo_provider(demo_provider),
          test_greeting("Hello, I am a test",                   // text
                        this->demo_provider->identity(),        // identity
                        this->demo_provider->implementation(),  // implementation
                        this->demo_provider->birth(),           // birth
                        {
                            {"one", true},  // data
                            {"two", 2},
                            {"three", 3.141592653589793238},
                            {"four", "IV"},
                            {"five", cc::dt::Clock::now()},
                        })
    {
    }

    void DemoTest::SetUp()
    {
        this->demo_provider->initialize();
    }

    void DemoTest::TearDown()
    {
        this->demo_provider->deinitialize();
    }

    void DemoTest::test_got_provider()
    {
        ASSERT_TRUE(this->demo_provider);
    }

    void DemoTest::test_got_current_time()
    {
        time_t before = ::time(nullptr);

        demo::TimeData td = this->demo_provider->get_current_time();
        std::chrono::duration time_since_epoch = td.timepoint.time_since_epoch();
        auto duration_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch);
        time_t seconds = duration_seconds.count();

        time_t after = ::time(nullptr);

        EXPECT_GE(seconds, before);
        EXPECT_LE(seconds, after);

        std::tm expected_localtime;
        localtime_r(&seconds, &expected_localtime);
        EXPECT_EQ(td.localtime, expected_localtime);

        std::tm expected_utctime;
        gmtime_r(&seconds, &expected_utctime);
        EXPECT_EQ(td.utctime, expected_utctime);
    }

    void DemoTest::test_say_hello()
    {
        demo::Greeting received_greeting;

        auto signal_handle = demo::signal_greeting.connect(
            [&](cc::signal::MappingAction mapping_action,
                const std::string &identity,
                const demo::Greeting &greeting) {
                received_greeting = greeting;
            });

        this->demo_provider->start_watching();
        this->demo_provider->say_hello(this->test_greeting);
        this->demo_provider->stop_watching();

        demo::signal_greeting.disconnect(signal_handle);
        EXPECT_EQ(this->test_greeting, received_greeting);
    }

    void DemoTest::test_encode_decode_greeting()
    {
        auto encoded_greeting = ::cc::io::proto::encoded<cc::protobuf::demo::Greeting>(this->test_greeting);
        auto decoded_greeting = ::cc::io::proto::decoded<demo::Greeting>(encoded_greeting);
        EXPECT_EQ(this->test_greeting, decoded_greeting);
    }

    void DemoTest::test_encode_decode_timedata()
    {
        demo::TimeData td(cc::dt::Clock::now());
        auto encoded_td = ::cc::io::proto::encoded<cc::protobuf::demo::TimeData>(td);
        auto decoded_td = ::cc::io::proto::decoded<demo::TimeData>(encoded_td);
        EXPECT_EQ(td, decoded_td);
    }

}  // namespace demo
