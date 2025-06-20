// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client.h++
/// @brief Logging service - ZMQ client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-zmq-client-writer.h++"
#include "multilogger-zmq-common.h++"
#include "multilogger-api.h++"
#include "zmq-protobuf-client.h++"
#include "zmq-protobuf-messagewriter.h++"
#include "zmq-subscriber.h++"
#include "zmq-publisher.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    //==========================================================================
    // @class ClientImpl
    // @brief Handle requests from Multilogger ZMQ clients

    class ClientImpl
        : public API,
          public core::zmq::ProtoBufClient,
          public core::types::enable_create_shared<ClientImpl>
    {
        using This  = ClientImpl;
        using Super = API;

    protected:
        ClientImpl(const std::string &host = "");

    public:
        void initialize() override;
        void deinitialize() override;

        void submit(const core::types::Loggable::ptr &item) override;
        bool add_sink(const SinkSpec &spec) override;
        bool remove_sink(const SinkID &id) override;
        SinkSpec get_sink_spec(const SinkID &id) const override;
        SinkSpecs get_all_sink_specs() const override;
        SinkIDs list_sinks() const override;
        SinkTypes list_sink_types() const override;
        FieldNames list_message_fields() const override;
        FieldNames list_error_fields() const override;

        // Create a listener to read loggable items from server.
        std::shared_ptr<LogSource> listen(
            const ListenerSpec &spec) override;

    private:
        std::shared_ptr<core::zmq::Subscriber> subscriber;
        std::shared_ptr<core::zmq::Publisher> publisher;
        std::shared_ptr<ClientWriter> writer;
    };
}
