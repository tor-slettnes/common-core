// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-run.h++
/// @brief C++ multilogger - Launch ZeroMQ server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "multilogger-zmq-common.h++"

#include <memory>
#include <string>

namespace multilogger::zmq
{
    void run_service(
        std::shared_ptr<multilogger::API> api_provider,
        const std::string &bind_address = {});
}
