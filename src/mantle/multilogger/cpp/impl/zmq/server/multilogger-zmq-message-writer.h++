// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-message-writer.h++
/// @brief Server-side log reader that forwards log events to client over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-multilogger-types.h++"
#include "multilogger-api.h++"
#include "zmq-protobuf-signalwriter.h++"
#include "types/create-shared.h++"

namespace multilogger::zmq
{
    class MessageWriter
        : public core::zmq::ProtoBufSignalWriter<cc::platform::multilogger::Loggable>,
          public core::types::enable_create_shared<MessageWriter>
    {
        using This = MessageWriter;
        using Super = core::zmq::ProtoBufSignalWriter<cc::platform::multilogger::Loggable>;

    protected:
        MessageWriter(
            const std::shared_ptr<API> &provider,
            const std::shared_ptr<core::zmq::Publisher> &publisher);

        virtual ~MessageWriter();

    public:
        void initialize() override;
        void deinitialize() override;

    protected:
        void start();
        void stop();
        void worker();

        void on_log_item(const core::types::Loggable::ptr &msg);

    private:
        std::shared_ptr<API> provider;
        std::shared_ptr<LogSource> listener;
        bool keep_writing;
        std::thread worker_thread;
        ListenerSpec listener_spec;
    };
}  // namespace multilogger::zmq
