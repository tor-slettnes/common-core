// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-grpc.h++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sysconfig.h++"

namespace sysconfig::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace sysconfig::grpc
