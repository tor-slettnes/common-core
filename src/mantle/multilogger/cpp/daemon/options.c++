// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"

namespace multilogger
{
    Options::Options()
        : Super(),
          enable_grpc(false),
          enable_zmq(false)
    {
        this->describe("Log server.");
    }

    void Options::add_options()
    {
        Super::add_options();

#if USE_GRPC
        this->add_flag(
            {"--grpc", "--enable-grpc"},
            "Enable gRPC service",
            &this->enable_grpc,
            core::settings->get("enable grpc", true).as_bool());
#endif

#if USE_ZMQ
        this->add_flag(
            {"--zmq", "--enable-zmq"},
            "Enable ZeroMQ service",
            &this->enable_zmq,
            core::settings->get("enable zmq", true).as_bool());
#endif
    }

    std::unique_ptr<Options> options;
}  // namespace multilogger
