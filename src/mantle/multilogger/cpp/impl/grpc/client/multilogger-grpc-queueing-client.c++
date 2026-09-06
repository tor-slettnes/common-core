// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-queuing-client.c++
/// @brief MultiLogger gRPC client that submits messages in the background.
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-grpc-queueing-client.h++"

namespace cc::platform::multilogger::grpc
{
    QueueingClient::~QueueingClient()
    {
        this->stop_worker();
    }

    void QueueingClient::initialize()
    {
        Super::initialize();
        this->start_worker();
    }

    void QueueingClient::deinitialize()
    {
        this->stop_worker();
        Super::deinitialize();
    }

    void QueueingClient::submit(const core::types::Loggable::ptr &item)
    {
        this->start_worker();
        this->submission_queue_.put(item);
    }

    void QueueingClient::start_worker()
    {
        if (!this->writer_thread_.joinable())
        {
            this->writer_thread_ = std::thread(&This::write_worker, this);
        }
    }
    void QueueingClient::stop_worker()
    {
        if (this->writer_thread_.joinable())
        {
            this->submission_queue_.close();
            this->writer_thread_.join();
        }
    }

    void QueueingClient::write_worker()
    {
        while (auto opt_item = this->submission_queue_.get())
        {
            this->write_item(opt_item.value());
        }
    }
}  // namespace cc::platform::multilogger::grpc
