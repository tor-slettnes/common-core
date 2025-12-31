// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-requesthandler.h++
/// @brief Handle MultiLogger control requests received from clients over ZMQ
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "multilogger-zmq-common.h++"
#include "zmq-protobuf-requesthandler.h++"

#include "cc/platform/multilogger/protobuf/multilogger_types.pb.h"

#include <memory>

namespace multilogger::zmq
{
    class RequestHandler : public core::zmq::ProtoBufRequestHandler,
                           public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::zmq::ProtoBufRequestHandler;

    protected:
        RequestHandler(const std::shared_ptr<API> &api,
                       const std::string &interface_name = MULTILOGGER_INTERFACE);

        void initialize() override;

        void add_sink(
            const cc::platform::multilogger::protobuf::SinkSpec &request,
            cc::platform::multilogger::protobuf::AddSinkResult *response);

        void remove_sink(
            const cc::platform::multilogger::protobuf::SinkID &request,
            cc::platform::multilogger::protobuf::RemoveSinkResult *response);

        void get_sink(
            const cc::platform::multilogger::protobuf::SinkID &request,
            cc::platform::multilogger::protobuf::SinkSpec *response);

        void get_all_sinks(
            const google::protobuf::Empty &,
            cc::platform::multilogger::protobuf::SinkSpecs *response);

        void list_sinks(
            const google::protobuf::Empty &,
            cc::platform::multilogger::protobuf::SinkNames *response);

        void list_sink_types(
            const google::protobuf::Empty &,
            cc::platform::multilogger::protobuf::SinkTypes *response);

        void list_message_fields(
            const google::protobuf::Empty &,
            cc::platform::multilogger::protobuf::FieldNames *response);

        void list_error_fields(
            const google::protobuf::Empty &,
            cc::platform::multilogger::protobuf::FieldNames *response);

    private:
        std::shared_ptr<API> provider;
    };

}  // namespace multilogger::zmq
