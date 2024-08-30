// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-grpc.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig.h++"

namespace platform::sysconfig::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace platform::sysconfig::grpc
