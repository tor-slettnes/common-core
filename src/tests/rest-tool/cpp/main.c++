// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "argparse/common.h++"
#include "application/init.h++"
#include "http-client.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    shared::application::initialize(argc, argv);

    auto options = std::make_unique<shared::argparse::CommonOptions>(false);
    options->apply(argc, argv);

    auto client = shared::http::HTTPClient();
    std::stringstream header, content;

    shared::http::ResponseCode response_code = client.get(
        "https://api.ipify.org?format=json",  // location
        &header,                                 // header_stream
        &content,                                // content_stream,
        true);                                   // fail_on_error

    std::cout << "### Received response " << response_code << std::endl;
    std::cout << "--- Header:" << std::endl;
    std::cout << header.rdbuf() << std::endl;
    std::cout << "--- Content:" << std::endl;
    std::cout << content.rdbuf() << std::endl;
    std::cout << "---" << std::endl;

    shared::application::deinitialize();
    return 0;
}
