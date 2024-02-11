// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for demo tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-provider.h++"

namespace cc::demo
{
    Options::Options(const std::string &implementation)
        : Super(),
          implementation(implementation)
    {
        this->describe("Example Tool.");
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
            {"[time|greetings|all]"},
            "Stay alive and watch for notification events from the underlying "
            "implementation. If one or more types are listed, only watch "
            "for the corresponding event type.",
            std::bind(&Options::monitor, this));
    }

    void Options::on_monitor_start()
    {
        FlagMap flags;
        bool &show_time = flags["time"];
        bool &show_greetings = flags["greetings"];
        bool &show_all = flags["all"];
        this->get_flags(&flags, false);

        if (show_all || (!show_time && !show_greetings))
        {
            show_time = show_greetings = true;
        }

        // Connect signals from `demo/signals.hpp` to our callback
        // functions.

        using namespace std::placeholders;
        if (show_time)
        {
            // Invoke `on_time_update` whenever there is a time update.
            // This signal is based on the `cc::signal::Signal<>` template,
            // so the callback function will receive one argument: the payload.
            cc::demo::signal_time.connect(
                this->identity,
                std::bind(&Options::on_time, this, _1));
        }

        if (show_greetings)
        {
            // Invoke `on_greeting_update` whenever someone sends a greeting.
            // This signal is based on `cc::signal::MappingSignal<>`, so
            // the callback function will receive three arguments:
            //   - The change type (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
            //   - The key (in this case we use the greeter's identity)
            //   - The payload.

            cc::demo::signal_greeting.connect(
                this->identity,
                std::bind(&Options::on_greeting, this, _1, _2, _3));
        }

        cc::demo::provider->start_watching();
    }

    void Options::on_monitor_end()
    {
        cc::demo::provider->stop_watching();
        cc::demo::signal_time.disconnect(this->identity);
        cc::demo::signal_greeting.disconnect(this->identity);
    }

    std::unique_ptr<Options> options;
}  // namespace cc::demo
