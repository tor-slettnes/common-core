// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-queuing-client.h++
/// @brief MultiLogger gRPC client that submits messages in the background.
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "multilogger-grpc-client.h++"
#include "thread/blockingqueue.h++"

namespace cc::platform::multilogger::grpc
{
    constexpr std::size_t SUBMISSION_QUEUE_SIZE = 4096;

    class QueueingClient
        : public Client,
          public core::types::enable_create_shared_from_this<QueueingClient>
    {
        using This = QueueingClient;
        using Super = Client;

    protected:
        template <class... Args>
        QueueingClient(
            const std::string &host = "",
            Args &&...args)
            : Super(host, std::forward<Args>(args)...),
              submission_queue_(SUBMISSION_QUEUE_SIZE)
        {
        }

        ~QueueingClient();

    public:
        void initialize() override;
        void deinitialize() override;
        void submit(const core::types::Loggable::ptr &item) override;

    private:
        void start_worker();
        void stop_worker();
        void write_worker();

    private:
        core::types::BlockingQueue<core::types::Loggable::ptr> submission_queue_;
        std::thread writer_thread_;
    };
}  // namespace cc::platform::multilogger::grpc
