// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief Implements demo commmands
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-provider.h++"
#include "string/convert.h++"
#include "chrono/date-time.h++"

namespace cc::demo
{
    void Options::say_hello()
    {
        std::string text = this->pop_arg("text");
        cc::types::KeyValueMap data = this->pop_attributes(false);

        cc::demo::Greeting greeting(
            text,                                         // text
            this->identity,                               // identity
            cc::str::convert_from(this->implementation),  // implementation
            cc::dt::Clock::now(),                         // birth
            data);                                        // data

        cc::demo::provider->say_hello(greeting);
    }

    void Options::get_current_time()
    {
        std::cout << cc::demo::provider->get_current_time() << std::endl;
    }

    void Options::start_ticking()
    {
        cc::demo::provider->start_ticking();
    }

    void Options::stop_ticking()
    {
        cc::demo::provider->stop_ticking();
    }

    void Options::on_time(const cc::demo::TimeData &timedata)
    {
        cc::str::format(std::cout,
                        "signal_time(%s)\n",
                        timedata);
    }

    void Options::on_greeting(cc::signal::MappingChange change,
                              const std::string &identity,
                              const cc::demo::Greeting &greeting)
    {
        cc::str::format(std::cout,
                        "signal_greeting(%s, %r, %s)\n",
                        change,
                        identity,
                        greeting);
    }
}  // namespace cc::demo
