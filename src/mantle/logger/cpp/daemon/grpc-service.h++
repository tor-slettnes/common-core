// -*- c++ -*-
//==============================================================================
/// @file grpc-service.h++
/// @brief Launch Logger gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-api.h++"

#include <memory>
#include <string>

namespace logger::grpc
{
    void run_service(
        std::shared_ptr<API> logger_provider,
        const std::string &listen_address = {});
}  // namespace logger::grpc
