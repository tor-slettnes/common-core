// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-grpc.h++
/// @brief UPGRADE service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <string>

namespace platform::upgrade::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace platform::upgrade::grpc
