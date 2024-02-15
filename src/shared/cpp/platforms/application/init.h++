// -*- c++ -*-
//==============================================================================
/// @file init.h++
/// @brief Common initialization tasks
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "platform/init.h++"

namespace shared::application
{
    void initialize(int argc, char **argv);
    void deinitialize();
}  // namespace shared::application
