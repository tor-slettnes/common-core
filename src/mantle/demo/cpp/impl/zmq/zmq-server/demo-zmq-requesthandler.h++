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

#include "demo.pb.h"

#include <memory>

namespace demo::zmq
{
    class RequestHandler : public cc::zmq::ProtoBufRequestHandler,
                           public cc::types::enable_create_shared<RequestHandler>
    {
        using Super = cc::zmq::ProtoBufRequestHandler;

    protected:
        RequestHandler(const std::shared_ptr<API> &api,
                       const std::string interface_name);

        void initialize() override;

        ::google::protobuf::Empty say_hello(const cc::protobuf::demo::Greeting &request);
        cc::protobuf::demo::TimeData get_current_time(const ::google::protobuf::Empty &request);
        ::google::protobuf::Empty start_ticking(const ::google::protobuf::Empty &);
        ::google::protobuf::Empty stop_ticking(const ::google::protobuf::Empty &);

    private:
        std::shared_ptr<API> api;

    };

}  // namespace demo::zmq
