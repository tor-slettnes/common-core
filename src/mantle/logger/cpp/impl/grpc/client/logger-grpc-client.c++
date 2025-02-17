// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-client.c++
/// @brief Logger gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-client.h++"
#include "logger-grpc-clientlistener.h++"
#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

namespace logger::grpc
{
    void LoggerClient::initialize()
    {
        LoggerClientBase::initialize();
        if (this->add_local_sink)
        {
            this->open();
            core::logging::dispatcher->add_sink(this->shared_from_this());
        }
    }

    void LoggerClient::deinitialize()
    {
        if (this->add_local_sink)
        {
            core::logging::dispatcher->remove_sink(this->identity());
            this->close();
        }
        LoggerClientBase::deinitialize();
    }

    void LoggerClient::submit(const core::status::Event::ptr &event)
    {
        this->call_check(
            &Stub::submit,
            ::protobuf::encoded<cc::status::Event>(event));
    }

    bool LoggerClient::add_sink(const SinkSpec &spec)
    {
        return this->call_check(
                       &Stub::add_sink,
                       ::protobuf::encoded<cc::logger::SinkSpec>(spec))
            .added();
    }

    bool LoggerClient::remove_sink(const SinkID &id)
    {
        return this->call_check(
                       &Stub::remove_sink,
                       ::protobuf::encoded<cc::logger::SinkID>(id))
            .removed();
    }

    SinkSpec LoggerClient::get_sink_spec(const SinkID &id) const
    {
        return protobuf::decoded<SinkSpec>(
            this->call_check(
                &Stub::get_sink,
                ::protobuf::encoded<cc::logger::SinkID>(id)));
    }

    SinkSpecs LoggerClient::get_all_sink_specs() const
    {
        return protobuf::decoded<SinkSpecs>(
            this->call_check(&Stub::get_all_sinks));
    }

    SinkIDs LoggerClient::list_sinks() const
    {
        return protobuf::assign_to_vector<SinkID>(
            this->call_check(&Stub::list_sinks).sink_names());
    }

    SinkTypes LoggerClient::list_sink_types() const
    {
        return protobuf::assign_to_vector<SinkType>(
            this->call_check(&Stub::list_sink_types).sink_types());
    }

    FieldNames LoggerClient::list_static_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::list_static_fields).field_names());
    }

    std::shared_ptr<EventSource> LoggerClient::listen(const ListenerSpec &spec)
    {
        return ClientListener::create_shared(this->stub, spec);
    }

    void LoggerClient::open()
    {
        AsyncLogSink::open();
        if (!this->writer)
        {
            this->writer_context = std::make_unique<::grpc::ClientContext>();
            this->writer_response = std::make_unique<::google::protobuf::Empty>();
            this->writer = this->stub->writer(this->writer_context.get(),
                                              this->writer_response.get());
        }
    }

    void LoggerClient::close()
    {
        if (this->writer)
        {
            this->writer->WritesDone();
            this->writer_status = this->writer->Finish();
            this->writer.reset();
            this->writer_context.reset();
        }
        AsyncLogSink::close();
    }

    void LoggerClient::capture_event(const core::status::Event::ptr &event)
    {
        if (!this->writer->Write(protobuf::encoded<cc::status::Event>(event)))
        {
            this->close();
        }
    }

}  // namespace logger::grpc
