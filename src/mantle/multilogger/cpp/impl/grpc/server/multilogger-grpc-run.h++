// -*- c++ -*-
//==============================================================================
/// @file grpc-service.h++
/// @brief Launch MultiLogger gRPC services
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "multilogger-api.h++"

#include <memory>
#include <string>

namespace multilogger::grpc
{
    void run_service(
        std::shared_ptr<API> multilogger_provider,
        const std::string &listen_address = {});
}  // namespace multilogger::grpc
