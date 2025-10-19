// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-requesthandler.h++
/// @brief Handle MultiLogger control requests received from clients over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "multilogger-zmq-common.h++"
#include "zmq-protobuf-requesthandler.h++"

#include "cc/protobuf/multilogger/multilogger.pb.h"

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
            const cc::platform::multilogger::SinkSpec &request,
            cc::platform::multilogger::AddSinkResult *response);

        void remove_sink(
            const cc::platform::multilogger::SinkID &request,
            cc::platform::multilogger::RemoveSinkResult *response);

        void get_sink(
            const cc::platform::multilogger::SinkID &request,
            cc::platform::multilogger::SinkSpec *response);

        void get_all_sinks(
            const google::protobuf::Empty &,
            cc::platform::multilogger::SinkSpecs *response);

        void list_sinks(
            const google::protobuf::Empty &,
            cc::platform::multilogger::SinkNames *response);

        void list_sink_types(
            const google::protobuf::Empty &,
            cc::platform::multilogger::SinkTypes *response);

        void list_message_fields(
            const google::protobuf::Empty &,
            cc::platform::multilogger::FieldNames *response);

        void list_error_fields(
            const google::protobuf::Empty &,
            cc::platform::multilogger::FieldNames *response);

    private:
        std::shared_ptr<API> provider;
    };

}  // namespace multilogger::zmq
