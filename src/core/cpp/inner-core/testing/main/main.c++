// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief C++ core - test routines
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"
#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    core::application::initialize(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
