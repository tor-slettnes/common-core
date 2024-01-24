// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-server.h++
/// @brief Demo gRPC server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"

#include <memory>
#include <string>

namespace cc::demo::grpc
{
    void run_grpc_service(
        std::shared_ptr<cc::demo::API> api_provider);
}  // namespace cc::demo::grpc
