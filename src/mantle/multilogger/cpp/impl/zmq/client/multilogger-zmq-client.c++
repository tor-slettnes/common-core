// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client.c++
/// @brief Logging service - ZMQ client implementation
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-client.h++"
#include "multilogger-zmq-client-reader.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::multilogger::zmq
{
    ClientImpl::ClientImpl(const std::string& host)
        : ProtoBufClient(
              host,
              SERVICE_CHANNEL,
              MULTILOGGER_INTERFACE,
              Role::SATELLITE),
          host_(host)
    {
    }

    void ClientImpl::initialize()
    {
        API::initialize();
        cc::zmq::ProtoBufClient::initialize();
    }

    void ClientImpl::deinitialize()
    {
        if (auto writer = this->writer_)
        {
            writer->deinitialize();
        }

        if (auto publisher = this->publisher_)
        {
            publisher->deinitialize();
        }

        if (auto subscriber = this->subscriber_)
        {
            subscriber->deinitialize();
        }

        cc::zmq::ProtoBufClient::deinitialize();
        API::deinitialize();
    }

    void ClientImpl::submit(const core::types::Loggable::ptr& item)
    {
        this->writer()->write(item);
    }

    bool ClientImpl::add_sink(const SinkSpec& spec)
    {
        auto result = this->call<cc::platform::multilogger::protobuf::AddSinkResult>(
            METHOD_ADD_SINK,
            cc::protobuf::encoded<cc::platform::multilogger::protobuf::SinkSpec>(spec));

        return result.added();
    }

    bool ClientImpl::remove_sink(const SinkID& id)
    {
        auto result = this->call<cc::platform::multilogger::protobuf::RemoveSinkResult>(
            METHOD_REMOVE_SINK,
            cc::protobuf::encoded<cc::platform::multilogger::protobuf::SinkID>(id));

        return result.removed();
    }

    SinkSpec ClientImpl::get_sink_spec(const SinkID& id) const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::SinkSpec>(
            METHOD_GET_SINK,
            cc::protobuf::encoded<cc::platform::multilogger::protobuf::SinkID>(id));

        return cc::protobuf::decoded<SinkSpec>(result);
    }

    SinkSpecs ClientImpl::get_all_sink_specs() const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::SinkSpecs>(
            METHOD_GET_ALL_SINKS);

        return cc::protobuf::decoded<SinkSpecs>(result);
    }

    SinkIDs ClientImpl::list_sinks() const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::SinkNames>(
            METHOD_LIST_SINKS);

        return cc::protobuf::assign_to_vector<SinkID>(result.sink_names());
    }

    SinkTypes ClientImpl::list_sink_types() const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::SinkTypes>(
            METHOD_LIST_SINK_TYPES);

        return cc::protobuf::assign_to_vector<std::string>(result.sink_types());
    }

    FieldNames ClientImpl::list_message_fields() const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::FieldNames>(
            METHOD_LIST_MESSAGE_FIELDS);
        return cc::protobuf::assign_to_vector<std::string>(result.field_names());
    }

    FieldNames ClientImpl::list_error_fields() const
    {
        auto result = this->call<cc::platform::multilogger::protobuf::FieldNames>(
            METHOD_LIST_ERROR_FIELDS);
        return cc::protobuf::assign_to_vector<std::string>(result.field_names());
    }

    std::shared_ptr<LogSource> ClientImpl::listen(
        const ListenerSpec& spec)
    {
        auto reader = ClientReader::create_shared(this->subscriber());
        reader->initialize();
        return reader;
    }

    std::shared_ptr<cc::zmq::Subscriber> ClientImpl::subscriber()
    {
        if (!this->subscriber_)
        {
            this->subscriber_ = std::make_shared<cc::zmq::Subscriber>(
                this->host_,
                MONITOR_CHANNEL,
                Role::SATELLITE);
            this->subscriber_->initialize();
        }

        return this->subscriber_;
    }

    std::shared_ptr<cc::zmq::Publisher> ClientImpl::publisher()
    {
        if (!this->publisher_)
        {
            this->publisher_ = std::make_shared<cc::zmq::Publisher>(
                this->host_,
                SUBMIT_CHANNEL,
                Role::SATELLITE);
            this->publisher_->initialize();
        }
        return this->publisher_;
    }

    std::shared_ptr<ClientWriter> ClientImpl::writer()
    {
        if (!this->writer_)
        {
            this->writer_ = ClientWriter::create_shared(this->publisher());
            this->writer_->initialize();
        }
        return this->writer_;
    }
}  // namespace cc::platform::multilogger::zmq
