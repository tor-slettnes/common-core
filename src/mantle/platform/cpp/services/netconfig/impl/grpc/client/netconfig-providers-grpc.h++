// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-grpc.h++
/// @brief NetConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string>

namespace netconfig::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace netconfig::grpc
