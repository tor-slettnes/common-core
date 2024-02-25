// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for demo tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "demo-api.h++"
#include "thread/signaltemplate.h++"

namespace demo
{
    class Options : public shared::argparse::CommandOptions
    {
        using Super = shared::argparse::CommandOptions;

    public:
        Options(const std::string &implementation);

    private:
        void add_options() override;
        void add_commands();

        void on_monitor_start() override;
        void on_monitor_end() override;

        void on_time(const demo::TimeData &time);

        void on_greeting(shared::signal::MappingChange change,
                         const std::string &identity,
                         const demo::Greeting &greeting);

        void say_hello();
        void get_current_time();

        void start_ticking();
        void stop_ticking();

    private:
        const std::string implementation;
    };

    extern std::unique_ptr<Options> options;

}  // namespace demo
