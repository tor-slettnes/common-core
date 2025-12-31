// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "settings/settings.h++"
#include "multilogger-grpc-clientsink.h++"

Options::Options()
    : Super()
{
    this->describe("Common Core platform server.");
}

void Options::add_options()
{
    Super::add_options();

    this->add_opt(
        {"-h", "--host"},
        "ADDRESS",
        "Server address in the form [HOST][:PORT].",
        &this->host,
        core::settings->get("host").as_string());

}

std::unique_ptr<Options> options;
