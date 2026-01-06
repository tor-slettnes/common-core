// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-client.c++
/// @brief Relay gRPC client
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-client.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

namespace relay::grpc
{
    void Client::initialize()
    {
        ClientBase::initialize();
        Publisher::initialize();
        Subscriber::initialize();
    }

    void Client::deinitialize()
    {
        this->close_writer();
        Subscriber::deinitialize();
        Publisher::deinitialize();
        ClientBase::deinitialize();
    }

    bool Client::publish(const std::string &topic,
                         const core::types::Value &payload)
    {
        this->open_writer();

        Message message;
        message.set_topic(topic);
        protobuf::encode(payload, message.mutable_payload());

        if (this->writer_->Write(message))
        {
            return true;
        }
        else
        {
            this->close_writer();
            return false;
        }
    }

    void Client::open_writer()
    {
        if (!this->writer_)
        {
            this->writer_context_ = std::make_unique<::grpc::ClientContext>();
            this->writer_context_->set_wait_for_ready(true);
            this->writer_response_ = std::make_unique<::google::protobuf::Empty>();
            this->writer_ = this->stub->Writer(
                this->writer_context_.get(),
                this->writer_response_.get());
        }
    }

    void Client::close_writer()
    {
        if (this->writer_)
        {
            this->writer_->WritesDone();
            this->writer_status_ = this->writer_->Finish();
            this->writer_.reset();
            this->writer_context_.reset();
        }
    }

    void Client::start_reader()
    {
        if (!this->reader_thread_.joinable())
        {
            this->reader_ = this->create_reader({});
            this->reader_thread_ = std::thread(&This::read_worker, this);
        }
    }

    void Client::stop_reader()
    {
        if (this->reader_thread_.joinable())
        {
            this->reader_->close();
            this->reader_thread_.join();
        }
    }

    Reader::ptr Client::create_reader(
        const std::vector<std::string> &topics)
    {
        cc::platform::relay::protobuf::Filters filters;
        protobuf::assign_repeated(topics, filters.mutable_topics());
        return Reader::create_shared(this->stub, filters);
    }

    void Client::read_worker()
    {
        while (const auto &message_data = this->reader_->get())
        {
            relay::signal_message.emit(
                message_data->first,
                message_data->second);
        }
    }
}  // namespace relay::grpc
