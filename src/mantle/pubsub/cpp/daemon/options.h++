// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/server.h++"
#include "multilogger-api.h++"

namespace cc::platform::pubsub
{
    class Options : public core::argparse::ServerOptions
    {
        using This = Options;
        using Super = core::argparse::ServerOptions;

    public:
        Options();

        void add_options() override;

        void enact() override;

    private:
        void init_logging();

        void on_message(
            core::signal::MappingAction mapping_action,
            const std::string &topic,
            const core::types::Value &payload);

    private:
        std::string signal_handle;

    public:
        bool enable_grpc;
        bool enable_zmq;
        bool enable_logging;

        std::string zmq_producer_interface;
        std::string zmq_consumer_interface;

    private:
        std::string log_contract;
        std::string log_host;
        std::shared_ptr<multilogger::API> multilogger;
        multilogger::SinkSpec logsink_spec;
    };

}  // namespace cc::platform::pubsub
