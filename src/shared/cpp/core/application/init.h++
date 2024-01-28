// -*- c++ -*-
//==============================================================================
/// @file init.h++
/// @brief Common initialization tasks
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"

namespace cc::application
{
    void initialize(int argc, char **argv);
    void deinitialize();

    extern cc::signal::Signal<int> signal_shutdown;

}  // namespace cc::application
