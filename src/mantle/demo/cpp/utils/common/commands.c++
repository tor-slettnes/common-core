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

namespace demo
{
    void Options::say_hello()
    {
        std::string text = this->pop_arg("text");
        cc::types::KeyValueMap data = this->pop_attributes(false);

        demo::Greeting greeting(
            text,                                         // text
            this->identity,                               // identity
            cc::str::convert_from(this->implementation),  // implementation
            cc::dt::Clock::now(),                         // birth
            data);                                        // data

        demo::provider->say_hello(greeting);
    }

    void Options::get_current_time()
    {
        std::cout << demo::provider->get_current_time() << std::endl;
    }

    void Options::start_ticking()
    {
        demo::provider->start_ticking();
    }

    void Options::stop_ticking()
    {
        demo::provider->stop_ticking();
    }

    void Options::on_time(const demo::TimeData &timedata)
    {
        cc::str::format(std::cout,
                        "signal_time(%s)\n",
                        timedata);
    }

    void Options::on_greeting(cc::signal::MappingAction mapping_action,
                              const std::string &mapping_key,
                              const demo::Greeting &greeting)
    {
        cc::str::format(std::cout,
                        "signal_greeting(%s, %r, %s)\n",
                        mapping_action,
                        mapping_key,
                        greeting);
    }
}  // namespace demo
