// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief JSON parser test
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include "argparse/common.h++"
#include "json/jsondecoder.h++"
#include "json/jsonwriter.h++"
#include <iostream>

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    if (argc >= 2)
    {
        std::string json_string(argv[1]);
        std::cout << "parsing input: " << std::quoted(json_string) << std::endl;
        core::types::Value value = core::json::JsonDecoder::parse_text(std::move(json_string));
        std::cout << "parsed: " << value << std::endl;

        std::stringstream ss;
        core::json::JsonPrettyWriter writer(ss);
        writer.write(value);
        std::string reconstructed = ss.str();
        std::cout << "reconstructed:" << reconstructed << std::endl;

    }
    else
    {
        std::cerr << "Please provide a JSON string to parse" << std::endl;
    }

    core::application::deinitialize();
    return 0;
}
