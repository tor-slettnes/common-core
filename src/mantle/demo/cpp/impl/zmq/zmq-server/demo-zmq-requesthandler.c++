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
    RequestHandler::RequestHandler(const std::shared_ptr<API> &provider,
                                   const std::string &interface_name)
        : Super(interface_name),
          provider(provider)
    {
    }

    void RequestHandler::initialize()
    {
        Super::initialize();
        this->add_handler(METHOD_SAY_HELLO, &This::say_hello);
        this->add_handler(METHOD_GET_CURRENT_TIME, &This::get_current_time);
        this->add_handler(METHOD_START_TICKING, &This::start_ticking);
        this->add_handler(METHOD_STOP_TICKING, &This::stop_ticking);
    }

    void RequestHandler::say_hello(
        const cc::demo::Greeting &request,
        ::google::protobuf::Empty *)
    {
        this->provider->say_hello(::protobuf::decoded<Greeting>(request));
    }

    void RequestHandler::get_current_time(
        const ::google::protobuf::Empty &,
        cc::demo::TimeData *response)
    {
        protobuf::encode(this->provider->get_current_time(), response);
    }

    void RequestHandler::start_ticking(
        const ::google::protobuf::Empty &,
        ::google::protobuf::Empty *)
    {
        this->provider->start_ticking();
    }

    void RequestHandler::stop_ticking(
        const ::google::protobuf::Empty &,
        ::google::protobuf::Empty *)
    {
        this->provider->stop_ticking();
    }

}  // namespace demo::zmq
