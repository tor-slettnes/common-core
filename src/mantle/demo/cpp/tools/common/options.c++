// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for demo tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-provider.h++"

namespace demo
{
    Options::Options(const std::string &implementation)
        : Super(),
          implementation(implementation)
    {
        this->describe("Control DEMO service via " + implementation);
    }

    void Options::add_options()
    {
        Super::add_options();
        this->add_commands();
    }

    void Options::add_commands()
    {
        this->add_command(
            "say_hello",
            {"TEXT", "[DATA] ..."},
            "Greet others who may be listening.",
            std::bind(&Options::say_hello, this));

        this->add_command(
            "get_current_time",
            {},
            "Get current time",
            std::bind(&Options::get_current_time, this));

        this->add_command(
            "start_ticking",
            {},
            "Start scheduled task to send periodic time updates",
            std::bind(&Options::start_ticking, this));

        this->add_command(
            "stop_ticking",
            {},
            "Stop scheduled task to send periodic time updates",
            std::bind(&Options::stop_ticking, this));

        this->add_command(
            "monitor",
            {"[except]", "[time|greetings]"},
            "Stay alive and watch for notification events from the underlying "
            "implementation. If one or more types are listed, only watch "
            "for the corresponding event type.",
            std::bind(&Options::monitor, this));
    }

    void Options::on_monitor_start()
    {
        FlagMap flags;
        bool &except = flags["except"];
        bool &show_time = flags["time"];
        bool &show_greetings = flags["greetings"];
        this->get_flags(&flags, false);

        if (!show_time && !show_greetings)
        {
            except = true;
        }

        // Connect signals from `demo/signals.hpp` to our callback
        // functions.

        using namespace std::placeholders;
        if (show_time != except)
        {
            // Invoke `on_time_update` whenever there is a time update.
            // This signal is based on the `core::signal::DataSignal<>` template,
            // so the callback function will receive one argument: the payload.
            demo::signal_time.connect(
                this->identity,
                std::bind(&Options::on_time, this, _1));
        }

        if (show_greetings != except)
        {
            // Invoke `on_greeting_update` whenever someone sends a greeting.
            // This signal is based on `core::signal::MappingSignal<>`, so
            // the callback function will receive three arguments:
            //   - The mapping action (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
            //   - The mapping key (in this case we use the greeter's identity)
            //   - The payload.

            demo::signal_greeting.connect(
                this->identity,
                std::bind(&Options::on_greeting, this, _1, _2, _3));
        }

        demo::provider->start_watching();
    }

    void Options::on_monitor_end()
    {
        demo::provider->stop_watching();
        demo::signal_time.disconnect(this->identity);
        demo::signal_greeting.disconnect(this->identity);
    }

    std::unique_ptr<Options> options;
}  // namespace demo
