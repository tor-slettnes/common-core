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

        if (!this->submission_queue_.put(item))
        {
            core::str::format(
                std::cerr,
                "%s: submission queue full, unable to log item: %s\n",
                *this,
                *item);
        }
    }

    void QueueingClient::start_worker()
    {
        if (!this->writer_thread_.joinable())
        {
            core::str::format(
                std::cerr,
                "%s: starting write worker\n");
            this->writer_thread_ = std::thread(&This::write_worker, this);
        }
        else
        {
            core::str::format(
                std::cerr,
                "%s: not starting write worker\n");
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
            if (!this->write_item(opt_item.value()))
            {
                core::str::format(
                    std::cerr,
                    "%s: failed to stream item to MultiLogger service: %s\n",
                    *this,
                    *opt_item.value());
            }
        }
    }
}  // namespace cc::platform::multilogger::grpc
