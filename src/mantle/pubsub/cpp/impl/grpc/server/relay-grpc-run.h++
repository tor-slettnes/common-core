// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-run.h++
/// @brief Relay gRPC server
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <memory>
#include <string>

namespace cc::platform::pubsub::grpc
{
    void run_grpc_service(
        const std::string &listen_address = {});

}  // namespace cc::platform::pubsub::grpc
