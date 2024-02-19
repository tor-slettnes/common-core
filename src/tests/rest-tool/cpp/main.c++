// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "argparse/common.h++"
#include "application/init.h++"
#include "types/value.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "rest-client.h++"

int main(int argc, char** argv)
{
    shared::application::initialize(argc, argv);

    auto options = std::make_unique<shared::argparse::CommonOptions>(false);
    options->apply(argc, argv);

    auto client = shared::http::RESTClient("https://api.ipify.org", "");

    shared::http::ResponseCode response_code;
    std::string content_type;
    std::stringstream header, content;
    int status = 0;

    try
    {
        shared::types::Value response = client.get_json(
            "https://api.ipify.org?format=json");

        std::cout << "### Received response: " << std::endl
                  << response << std::endl
                  << "###" << std::endl;
    }
    catch (...)
    {
        std::cerr << std::current_exception() << std::endl;
        status = -1;
    }

    shared::application::deinitialize();
    return status;
}
