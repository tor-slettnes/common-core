// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-requesthandler.c++
/// @brief Handle MultiLogger control requests received from clients over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-request-handler.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    RequestHandler::RequestHandler(
        const std::shared_ptr<API> &provider,
        const std::string &interface_name)
        : Super(interface_name),
          provider(provider)
    {
    }

    void RequestHandler::initialize()
    {
        Super::initialize();
        this->add_handler(METHOD_ADD_SINK, &This::add_sink);
        this->add_handler(METHOD_REMOVE_SINK, &This::remove_sink);
        this->add_handler(METHOD_GET_SINK, &This::get_sink);
        this->add_handler(METHOD_GET_ALL_SINKS, &This::get_all_sinks);
        this->add_handler(METHOD_LIST_SINKS, &This::list_sinks);
        this->add_handler(METHOD_LIST_SINK_TYPES, &This::list_sink_types);
        this->add_handler(METHOD_LIST_MESSAGE_FIELDS, &This::list_message_fields);
        this->add_handler(METHOD_LIST_ERROR_FIELDS, &This::list_error_fields);
    }

    void RequestHandler::add_sink(
        const cc::platform::multilogger::protobuf::SinkSpec &request,
        cc::platform::multilogger::protobuf::AddSinkResult *response)
    {
        response->set_added(
            this->provider->add_sink(
                protobuf::decoded<multilogger::SinkSpec>(request)));
    }

    void RequestHandler::remove_sink(
        const cc::platform::multilogger::protobuf::SinkID &request,
        cc::platform::multilogger::protobuf::RemoveSinkResult *response)
    {
        response->set_removed(
            this->provider->remove_sink(
                request.sink_id()));
    }

    void RequestHandler::get_sink(
        const cc::platform::multilogger::protobuf::SinkID &request,
        cc::platform::multilogger::protobuf::SinkSpec *response)
    {
        protobuf::encode(
            this->provider->get_sink_spec(request.sink_id()),
            response);
    }

    void RequestHandler::get_all_sinks(
        const google::protobuf::Empty &,
        cc::platform::multilogger::protobuf::SinkSpecs *response)
    {
        protobuf::encode(
            this->provider->get_all_sink_specs(),
            response);
    }

    void RequestHandler::list_sinks(
        const google::protobuf::Empty &,
        cc::platform::multilogger::protobuf::SinkNames *response)
    {
        protobuf::assign_repeated(
            this->provider->list_sinks(),
            response->mutable_sink_names());
    }

    void RequestHandler::list_sink_types(
        const google::protobuf::Empty &,
        cc::platform::multilogger::protobuf::SinkTypes *response)
    {
        protobuf::assign_repeated(
            this->provider->list_sink_types(),
            response->mutable_sink_types());
    }

    void RequestHandler::list_message_fields(
        const google::protobuf::Empty &,
        cc::platform::multilogger::protobuf::FieldNames *response)
    {
        protobuf::assign_repeated(
            this->provider->list_message_fields(),
            response->mutable_field_names());
    }

    void RequestHandler::list_error_fields(
        const google::protobuf::Empty &,
        cc::platform::multilogger::protobuf::FieldNames *response)
    {
        protobuf::assign_repeated(
            this->provider->list_error_fields(),
            response->mutable_field_names());
    }

}  // namespace multilogger::zmq
