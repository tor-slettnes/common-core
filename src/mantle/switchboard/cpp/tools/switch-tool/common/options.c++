// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for switch control tool
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "platform/path.h++"

namespace switchboard
{
    Options::Options(const std::string &implementation)
        : core::argparse::CommandOptions(),
          implementation(implementation),
          signal_handle(core::platform::path->exec_name())
    {
        this->describe("Command line tool to access Switchboard service via " +
                       implementation);
    };

    void Options::add_options()
    {
        Super::add_options();

        this->add_void(
            {"--list-states"},
            "List available switch states and exit.",
            [&] {
                this->print_states();
                std::exit(0);
            });

        this->add_commands();
    }

    bool Options::handle_command(const std::shared_ptr<switchboard::Provider> &provider)
    {
        this->provider = provider;
        return Super::handle_command();
    }

    void Options::get_provider()
    {
        this->report_status_and_exit(this->provider->available());
    }

    std::unique_ptr<Options> options;
}  // namespace switchboard
