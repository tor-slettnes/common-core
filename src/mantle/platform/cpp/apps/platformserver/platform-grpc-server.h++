// -*- c++ -*-
//==============================================================================
/// @file grpc-service.h++
/// @brief Platform Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include <string>

namespace platform
{
    void run_grpc_service(const std::string &listen_address = "");
}  // namespace platform
