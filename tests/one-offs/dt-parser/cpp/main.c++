// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Date/TIme parser test
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include "argparse/common.h++"
#include "chrono/date-time.h++"
#include <iostream>

int main(int argc, char** argv)
{
    cc::application::initialize(argc, argv);
    std::string format = cc::dt::JS_FORMAT;

    if (argc >= 3)
    {
        format = argv[2];
    }

    if (argc >= 2)
    {
        std::string date_string(argv[1]);
        std::cout << "parsing input '" << date_string
                  << "' with format '" << format
                  << "':" << std::endl;

        cc::dt::TimePoint tp = cc::dt::to_timepoint(date_string, true, format);
        std::cout << "result: "
                  << tp
                  << std::endl;
    }
    else
    {
        std::cerr << "Please provide a date string and optional format string to parse"
                  << std::endl;
    }

    cc::application::deinitialize();
    return 0;
}
