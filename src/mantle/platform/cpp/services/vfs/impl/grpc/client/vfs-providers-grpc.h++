// -*- c++ -*-
//==============================================================================
/// @file vfs-providers-grpc.h++
/// @brief VFS service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string>

namespace vfs::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready = true,
                            bool start_watching = true);
    void unregister_providers();
}  // namespace vfs::grpc
