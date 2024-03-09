// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief JSON parser test
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include "argparse/common.h++"
#include "json/jsonparser.h++"
#include "json/jsonwriter.h++"
#include <iostream>

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    if (argc >= 2)
    {
        std::string json_string(argv[1]);
        std::cout << "parsing input: " << std::quoted(json_string) << std::endl;
        core::types::Value value = core::json::JsonParser::parse_text(json_string);
        std::cout << "parsed: " << value << std::endl;

        std::string reconstructed = core::json::JsonBuilder::to_string(value);
        std::cout << "reconstructed:" << reconstructed << std::endl;
    }
    else
    {
        std::cerr << "Please provide a JSON string to parse" << std::endl;
    }

    core::application::deinitialize();
    return 0;
}
