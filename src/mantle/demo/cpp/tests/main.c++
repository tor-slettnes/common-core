// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief C++ demo - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    cc::application::initialize(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
