// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-run.h++
/// @brief Run Switchboard gRPC server
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"

namespace switchboard::grpc
{
    void run_grpc_service(
        std::shared_ptr<switchboard::Provider> api_provider,
        const std::string &listen_address = {});
}  // namespace demo::grpc
