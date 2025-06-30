// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-client.h++
/// @brief Logging service - ZMQ client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-client.h++"
#include "multilogger-zmq-client-reader.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::zmq
{
    ClientImpl::ClientImpl(const std::string &host)
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
        core::zmq::ProtoBufClient::initialize();
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

        core::zmq::ProtoBufClient::deinitialize();
        API::deinitialize();
    }

    void ClientImpl::submit(const core::types::Loggable::ptr &item)
    {
        this->writer()->write(item);
    }

    bool ClientImpl::add_sink(const SinkSpec &spec)
    {
        auto result = this->call<cc::platform::multilogger::AddSinkResult>(
            METHOD_ADD_SINK,
            protobuf::encoded<cc::platform::multilogger::SinkSpec>(spec));

        return result.added();
    }

    bool ClientImpl::remove_sink(const SinkID &id)
    {
        auto result = this->call<cc::platform::multilogger::RemoveSinkResult>(
            METHOD_REMOVE_SINK,
            protobuf::encoded<cc::platform::multilogger::SinkID>(id));

        return result.removed();
    }

    SinkSpec ClientImpl::get_sink_spec(const SinkID &id) const
    {
        auto result = this->call<cc::platform::multilogger::SinkSpec>(
            METHOD_GET_SINK,
            protobuf::encoded<cc::platform::multilogger::SinkID>(id));

        return protobuf::decoded<SinkSpec>(result);
    }

    SinkSpecs ClientImpl::get_all_sink_specs() const
    {
        auto result = this->call<cc::platform::multilogger::SinkSpecs>(
            METHOD_GET_ALL_SINKS);

        return protobuf::decoded<SinkSpecs>(result);
    }

    SinkIDs ClientImpl::list_sinks() const
    {
        auto result = this->call<cc::platform::multilogger::SinkNames>(
            METHOD_LIST_SINKS);

        return protobuf::assign_to_vector<SinkID>(result.sink_names());
    }

    SinkTypes ClientImpl::list_sink_types() const
    {
        auto result = this->call<cc::platform::multilogger::SinkTypes>(
            METHOD_LIST_SINK_TYPES);

        return protobuf::assign_to_vector<std::string>(result.sink_types());
    }

    FieldNames ClientImpl::list_message_fields() const
    {
        auto result = this->call<cc::platform::multilogger::FieldNames>(
            METHOD_LIST_MESSAGE_FIELDS);
        return protobuf::assign_to_vector<std::string>(result.field_names());
    }

    FieldNames ClientImpl::list_error_fields() const
    {
        auto result = this->call<cc::platform::multilogger::FieldNames>(
            METHOD_LIST_ERROR_FIELDS);
        return protobuf::assign_to_vector<std::string>(result.field_names());
    }

    std::shared_ptr<LogSource> ClientImpl::listen(
        const ListenerSpec &spec)
    {
        auto reader = ClientReader::create_shared(this->subscriber());
        reader->initialize();
        return reader;
    }

    std::shared_ptr<core::zmq::Subscriber> ClientImpl::subscriber()
    {
        if (!this->subscriber_)
        {
            this->subscriber_ = std::make_shared<core::zmq::Subscriber>(
                this->host_,
                MONITOR_CHANNEL,
                Role::SATELLITE);
            this->subscriber_->initialize();
        }

        return this->subscriber_;
    }

    std::shared_ptr<core::zmq::Publisher> ClientImpl::publisher()
    {
        if (!this->publisher_)
        {
            this->publisher_ = std::make_shared<core::zmq::Publisher>(
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
}  // namespace multilogger::zmq
