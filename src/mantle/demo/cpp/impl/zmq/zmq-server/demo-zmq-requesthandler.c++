// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-requesthandler.c++
/// @brief C++ demo - Invoke handler methods based on ZeroMQ requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-requesthandler.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-inline.h++"

namespace demo::zmq
{
    RequestHandler::RequestHandler(const std::shared_ptr<API> &api,
                                   const std::string interface_name)
        : Super(interface_name),
          api(api)
    {
    }

    void RequestHandler::initialize()
    {
        Super::initialize();
        this->add_handler(METHOD_SAY_HELLO, &RequestHandler::say_hello);
        this->add_handler(METHOD_GET_CURRENT_TIME, &RequestHandler::get_current_time);
        this->add_handler(METHOD_START_TICKING, &RequestHandler::start_ticking);
        this->add_handler(METHOD_STOP_TICKING, &RequestHandler::stop_ticking);
    }

    ::google::protobuf::Empty RequestHandler::say_hello(const cc::demo::Greeting &request)
    {
        this->api->say_hello(::core::io::proto::decoded<Greeting>(request));
        return {};
    }

    cc::demo::TimeData RequestHandler::get_current_time(const ::google::protobuf::Empty &request)
    {
        return ::core::io::proto::encoded<cc::demo::TimeData>(this->api->get_current_time());
    }

    ::google::protobuf::Empty RequestHandler::start_ticking(const ::google::protobuf::Empty &)
    {
        this->api->start_ticking();
        return {};
    }

    ::google::protobuf::Empty RequestHandler::stop_ticking(const ::google::protobuf::Empty &)
    {
        this->api->stop_ticking();
        return {};
    }

}

