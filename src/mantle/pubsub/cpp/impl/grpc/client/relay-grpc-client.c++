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

namespace pubsub::grpc
{
    void Client::initialize()
    {
        ClientBase::initialize();
        Publisher::initialize();
        Subscriber::initialize();
    }

    void Client::deinitialize()
    {
        Subscriber::deinitialize();
        Publisher::deinitialize();
        ClientBase::deinitialize();
    }

    bool Client::write(const std::string &topic,
                       const core::types::Value &value)
    {
        cc::platform::pubsub::protobuf::Publication msg;
        msg.set_topic(topic);
        protobuf::encode(value, msg.mutable_value());
        return this->writer_->Write(msg);
    }

    void Client::start_writer()
    {
        if (!this->writer_)
        {
            this->writer_context_ = std::make_unique<::grpc::ClientContext>();
            this->writer_context_->set_wait_for_ready(true);
            this->writer_response_ = std::make_unique<::google::protobuf::Empty>();
            this->writer_ = this->stub->Publisher(
                this->writer_context_.get(),
                this->writer_response_.get());
        }
        pubsub::Publisher::start_writer();
    }

    void Client::stop_writer()
    {
        pubsub::Publisher::stop_writer();

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
        pubsub::Subscriber::start_reader();
    }

    void Client::stop_reader()
    {
        pubsub::Subscriber::stop_reader();
        if (this->reader_thread_.joinable())
        {
            this->reader_->close();
            this->reader_thread_.join();
        }
    }

    Reader::ptr Client::create_reader(
        const std::vector<std::string> &topics)
    {
        cc::platform::pubsub::protobuf::Filters filters;
        protobuf::assign_repeated(topics, filters.mutable_topics());
        return Reader::create_shared(this->stub, filters);
    }

    void Client::read_worker()
    {
        while (const auto &message_data = this->reader_->get())
        {
            pubsub::signal_publication.emit(
                message_data->first,
                message_data->second);
        }
    }
}  // namespace pubsub::grpc
