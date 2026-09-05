// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "relay-types.h++"
#include "multilogger-grpc-client.h++"
#include "multilogger-native.h++"
#include "logging/telemetry/data.h++"
#include "platform/init.h++"
#include "chrono/date-time.h++"

namespace cc::platform::pubsub
{
    Options::Options()
        : Super(),
          enable_grpc(false),
          enable_zmq(false)
    {
        this->describe("Message Relay.");
    }

    void Options::add_options()
    {
        Super::add_options();

#if USE_GRPC
        this->add_flag(
            {"--grpc", "--enable-grpc"},
            "Enable gRPC service",
            &this->enable_grpc,
            core::settings->get("enable grpc", true).as_bool());
#endif

#if USE_ZMQ
        this->add_flag(
            {"--enable-zmq"},
            "Enable ZeroMQ listener and broadcaster",
            &this->enable_zmq,
            core::settings->get("enable zmq", true).as_bool());

        this->add_opt(
            {"--zmq-producer-interface"},
            "ADDRESS",
            "Bind to specific interface address to listen for incoming ZMQ publications",
            &this->zmq_producer_interface);

        this->add_opt(
            {"--zmq-consumer-interface"},
            "ADDRESS",
            "Bind to specific interface addresss to broadcast outgoing ZMQ publications",
            &this->zmq_consumer_interface);
#endif
        core::types::Value log_options = core::settings->get("message logging");

        this->add_opt(
            {"--log-contract"},
            "CONTRACT_ID",
            "Log published messages via MultiLogger service. Default: %default."
            "See also `--log-host`.",
            &this->log_contract,
            log_options.get("contract").as_string());

        this->add_opt(
            {"--log-host"},
            "HOST",
            "Log published messages via MultiLogger service on HOST. "
            "If not specified, messages are logged locally. "
            "Requires `--log-contract`. "
            "Default: [%default].",
            &this->log_host,
            log_options.get("host").as_string());
    }

    void Options::enact()
    {
        Super::enact();
        this->init_logging();
    }

    void Options::init_logging()
    {
        if (!this->log_contract.empty())
        {
            if (!this->log_host.empty())
            {
                logf_info(
                    "Logging publications by contract %r via MultiLogger service on %s",
                    this->log_contract,
                    this->log_host);
                this->multilogger = multilogger::grpc::ClientImpl::create_shared(
                    this->log_host);
            }
            else
            {
                logf_info(
                    "Logging publications by contract %r locally.",
                    this->log_contract);
                this->multilogger = multilogger::native::Logger::create_shared();
            }

            using namespace std::placeholders;
            std::string signal_handle = signal_publication.connect(
                std::bind(&This::on_message, this, _1, _2, _3));

            core::platform::signal_shutdown.connect(
                [=] {
                    signal_publication.disconnect(signal_handle);
                });
        }
    }

    void Options::on_message(
        core::signal::MappingAction mapping_action,
        const std::string &topic,
        const core::types::Value &payload)
    {
        if (this->multilogger)
        {
            multilogger->submit(
                std::make_shared<core::logging::Data>(
                    this->log_contract,      // contract_id
                    core::dt::Clock::now(),  // tp
                    core::types::KeyValueMap{
                        // attributes
                        {"topic", topic},
                        {"payload", payload},
                    }));
        }
    }
}  // namespace cc::platform::pubsub
