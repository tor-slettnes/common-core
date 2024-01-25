// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-requesthandler.c++
/// @brief C++ demo - Invoke handler methods based on ZeroMQ requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-requesthandler.h++"
#include "protobuf-demo-types.h++"
#include "protobuf-inline.h++"

namespace cc::demo::zmq
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

    protobuf::Empty RequestHandler::say_hello(const CC::Demo::Greeting &request)
    {
        this->api->say_hello(protobuf::decoded<Greeting>(request));
        return {};
    }

    CC::Demo::TimeData RequestHandler::get_current_time(const protobuf::Empty &request)
    {
        return protobuf::encoded<CC::Demo::TimeData>(this->api->get_current_time());
    }

    protobuf::Empty RequestHandler::start_ticking(const protobuf::Empty &)
    {
        this->api->start_ticking();
        return {};
    }

    protobuf::Empty RequestHandler::stop_ticking(const protobuf::Empty &)
    {
        this->api->stop_ticking();
        return {};
    }

}

