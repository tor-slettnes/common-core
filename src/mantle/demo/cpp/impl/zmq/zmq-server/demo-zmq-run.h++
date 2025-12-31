// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-run.h++
/// @brief C++ demo - Launch ZeroMQ server implementation
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "demo-api.h++"
#include "demo-zmq-common.h++"

#include <memory>
#include <string>

namespace demo::zmq
{
    void run_zmq_service(
        std::shared_ptr<demo::API> api_provider,
        const std::string &bind_address = {});
}
