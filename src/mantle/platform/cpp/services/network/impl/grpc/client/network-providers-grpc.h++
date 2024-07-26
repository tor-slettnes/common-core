// -*- c++ -*-
//==============================================================================
/// @file network-providers-grpc.h++
/// @brief Network service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <string>

namespace platform::network::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace platform::network::grpc
