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
    class RequestHandler : public core::zmq::ProtoBufRequestHandler,
                           public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::zmq::ProtoBufRequestHandler;

    protected:
        RequestHandler(const std::shared_ptr<API> &provider,
                       const std::string &interface_name = DEMO_INTERFACE);

        void initialize() override;

        void say_hello(
            const cc::demo::Greeting &request,
            ::google::protobuf::Empty *);

        void get_current_time(
            const ::google::protobuf::Empty &,
            cc::demo::TimeData *response);

        void start_ticking(
            const ::google::protobuf::Empty &,
            ::google::protobuf::Empty *);

        void stop_ticking(
            const ::google::protobuf::Empty &,
            ::google::protobuf::Empty *);

    private:
        std::shared_ptr<API> provider;
    };

}  // namespace demo::zmq
