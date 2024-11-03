// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-client.c++
/// @brief Logger gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-client.h++"
#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

namespace logger::grpc
{
    void LoggerClient::initialize()
    {
        ClientBase::initialize();
        if (this->add_local_sink)
        {
            this->open();
            core::logging::message_dispatcher.add_sink(this->shared_from_this());
        }
    }

    void LoggerClient::deinitialize()
    {
        if (this->add_local_sink)
        {
            core::logging::message_dispatcher.remove_sink(this->identity());
            this->close();
        }
        ClientBase::deinitialize();
    }

    void LoggerClient::log(const core::status::Event::ptr &event)
    {
        this->call_check(
            &Stub::log,
            ::protobuf::encoded_event(event));
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

    SinkSpecs LoggerClient::list_sinks() const
    {
        return protobuf::decoded<SinkSpecs>(
            this->call_check(
                &Stub::list_sinks));
    }

    FieldNames LoggerClient::list_static_fields() const
    {
        return protobuf::decoded<FieldNames>(
            this->call_check(
                &Stub::list_static_fields));
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
        if (!this->writer->Write(protobuf::encoded_event(event)))
        {
            this->close();
        }
    }

}  // namespace logger::grpc
