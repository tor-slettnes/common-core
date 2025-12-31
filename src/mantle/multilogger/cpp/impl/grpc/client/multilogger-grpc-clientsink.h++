/// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-clientsink.c++
/// @brief Log to remote MultiLogger service
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-grpc-client.h++"
#include "logging/sinks/sink.h++"
#include "logging/sinks/async-wrapper.h++"
#include "logging/sinks/factory.h++"
#include "types/create-shared.h++"

namespace multilogger::grpc
{
    class ClientSink
        : public core::logging::AsyncWrapper<core::logging::Sink>,
          public core::types::enable_create_shared<ClientSink>
    {
        using This = ClientSink;
        using Super = core::logging::AsyncWrapper<core::logging::Sink>;

    protected:
        ClientSink(const std::string &sink_id);

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
        std::shared_ptr<ClientImpl> client_;
        std::string host_;
        std::size_t queue_size_;
    };

    inline static core::logging::SinkFactory multilogger_factory(
        "multilogger",
        "Log to MultiLogger service",
        [](const core::logging::SinkID &sink_id) -> core::logging::Sink::ptr
        {
            return ClientSink::create_shared(sink_id);
        });
}  // namespace multilogger
