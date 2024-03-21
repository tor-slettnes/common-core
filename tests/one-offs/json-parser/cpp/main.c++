// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief JSON parser test
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include "argparse/common.h++"
#include "json/reader.h++"
#include "json/writer.h++"
#include <iostream>

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    if (argc >= 2)
    {
        std::string json_string(argv[1]);
        std::cout << "parsing input: '" << json_string << "'" << std::endl;
        core::types::Value value = core::json::Reader().decoded(json_string);
        std::cout << "parsed: " << value << std::endl;

        std::string reconstructed = core::json::Writer().encoded(value, true);
        std::cout << "reconstructed:" << reconstructed << std::endl;
    }
    else
    {
        std::cerr << "Please provide a JSON string to parse" << std::endl;
    }
    return 0;
}
