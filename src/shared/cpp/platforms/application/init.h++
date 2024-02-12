// -*- c++ -*-
//==============================================================================
/// @file init.h++
/// @brief Common initialization tasks
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"

namespace shared::application
{
    void initialize(int argc, char **argv);
    void deinitialize();

    extern shared::signal::Signal<int> signal_shutdown;

}  // namespace shared::application
