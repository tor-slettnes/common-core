// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-requesthandler.h++
/// @brief C++ demo - Invoke handler methods based on ZeroMQ requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"
#include "demo-zmq-common.h++"
#include "zmq-protobuf-requesthandler.h++"

#include "demo_types.pb.h"

#include <memory>

namespace cc::demo::zmq
{
    class RequestHandler : public cc::zmq::ProtoBufRequestHandler,
                           public types::enable_create_shared<RequestHandler>
    {
        using Super = cc::zmq::ProtoBufRequestHandler;

    protected:
        RequestHandler(const std::shared_ptr<API> &api,
                       const std::string interface_name);

        void initialize() override;
        void deinitialize() override;

        protobuf::Empty say_hello(const CC::Demo::Greeting &request);
        CC::Demo::TimeData get_current_time(const protobuf::Empty &request);
        protobuf::Empty start_ticking(const protobuf::Empty &);
        protobuf::Empty stop_ticking(const protobuf::Empty &);

    private:
        std::shared_ptr<API> api;

    };

}  // namespace cc::demo::zmq
