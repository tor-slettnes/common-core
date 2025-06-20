// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client-sink.h++
/// @brief Log sink that forwards messages to MultiLogger over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-zmq-client-writer.h++"
#include "logging/sinks/sink.h++"
#include "logging/sinks/async-wrapper.h++"
#include "logging/sinks/factory.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    const std::string SETTING_HOST = "host";
    const std::string DEFAULT_HOST = "localhost";

    //--------------------------------------------------------------------------
    // @class ClientWriter
    // @brief Send messages to Multilogger service over ZMQ

    class ClientSink
        : public core::logging::AsyncWrapper<core::logging::Sink>,
          public core::types::enable_create_shared_from_this<ClientSink>
    {
        using This = ClientSink;
        using Super = core::logging::AsyncWrapper<core::logging::Sink>;

    protected:
        ClientSink(const std::string &sink_id,
                   const std::shared_ptr<core::zmq::Publisher> &publisher = {});

    protected:
        void load_settings(const core::types::KeyValueMap &settings) override;
        void load_client_settings(const core::types::KeyValueMap &settings);

    public:
        std::string host() const;
        void set_host(const std::string &address);

    protected:
        void open() override;
        void close() override;
        bool handle_item(const core::types::Loggable::ptr &loggable);

    private:
        std::string host_;
        bool external_publisher_;
        std::shared_ptr<core::zmq::Publisher> publisher_;
        std::shared_ptr<ClientWriter> writer_;
    };

    //--------------------------------------------------------------------------
    // Sink factory

    inline static core::logging::SinkFactory factory(
        "multilogger-zmq",
        "Log to MultiLogger service over ZeroMQ",
        [](const core::logging::SinkID &sink_id) -> core::logging::Sink::ptr {
            return ClientSink::create_shared(sink_id);
        });

}  // namespace multilogger::zmq
