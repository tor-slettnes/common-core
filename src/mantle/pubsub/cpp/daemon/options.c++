// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "relay-types.h++"
#include "multilogger-grpc-queueing-client.h++"
#include "multilogger-native.h++"
#include "logging/telemetry/data.h++"
#include "platform/init.h++"
#include "platform/symbols.h++"
#include "chrono/date-time.h++"

namespace cc::platform::pubsub
{
    Options::Options()
        : Super(),
          signal_handle(TYPE_NAME_FULL(This)),
          enable_grpc(false),
          enable_zmq(false),
          enable_logging(false)
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
        this->add_flag(
            {"--enable-logging"},
            "Log published messages via MultiLogger interface. "
            "See also `--log-contract` and `--log-host`. "
            "Default: %default",
            &this->enable_logging,
            log_options.get("enable", false).as_bool());

        this->add_opt(
            {"--log-contract"},
            "CONTRACT_ID",
            "Log contract identifier for logging. Default: \"%default\". "
            "See also `--log-host`.",
            &this->log_contract,
            log_options.get("contract", "publication").as_string());

        this->add_opt(
            {"--log-host"},
            "HOST",
            "Log published messages via MultiLogger service on HOST. "
            "If empty, log messages locally. "
            "Default: \"%default\".",
            &this->log_host,
            log_options.get("host").as_string());
    }

    void Options::enact()
    {
        Super::enact();
        if (this->enable_logging)
        {
            this->init_logging();
        }
    }

    void Options::init_logging()
    {
        if (!this->log_host.empty())
        {
            logf_info(
                "Logging publications by contract %r via MultiLogger service on %s",
                this->log_contract,
                this->log_host);
            this->multilogger = multilogger::grpc::QueueingClient::create_shared(
                this->log_host);
        }
        else
        {
            logf_info(
                "Logging publications by contract %r locally.",
                this->log_contract);
            this->multilogger = multilogger::native::Logger::create_shared();
        }

        this->multilogger->initialize();

        using namespace std::placeholders;
        signal_publication.connect(
            this->signal_handle,
            std::bind(&This::on_message, this, _1, _2, _3));

        core::platform::signal_shutdown.connect(
            [=] {
                signal_publication.disconnect(this->signal_handle);
                this->multilogger->deinitialize();
            });
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
