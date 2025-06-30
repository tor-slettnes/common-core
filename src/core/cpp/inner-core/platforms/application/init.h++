// -*- c++ -*-
//==============================================================================
/// @file init.h++
/// @brief Common initialization tasks
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "platform/init.h++"

namespace core::application
{
    void initialize(int argc,
                    char **argv,
                    const std::optional<std::string> &flavor = {});
    void initialize_daemon(int argc, char **argv);

    void emit_shutdown_signal();
    void deinitialize();
}  // namespace core::application
