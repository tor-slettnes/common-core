// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-client.c++
/// @brief Log gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-client.h++"
#include "multilogger-grpc-clientlistener.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::grpc
{
    void LogClient::initialize()
    {
        LogClientBase::initialize();
        if (this->add_local_sink)
        {
            this->open();
            core::logging::dispatcher.add_sink(this->shared_from_this());
        }
    }

    void LogClient::deinitialize()
    {
        if (this->add_local_sink)
        {
            core::logging::dispatcher.remove_sink(this->identity());
            this->close();
        }
        LogClientBase::deinitialize();
    }

    void LogClient::submit(const core::status::Event::ptr &event)
    {
        this->call_check(
            &Stub::submit,
            ::protobuf::encoded<cc::status::Event>(event));
    }

    bool LogClient::add_sink(const SinkSpec &spec)
    {
        return this->call_check(
                       &Stub::add_sink,
                       ::protobuf::encoded<cc::multilogger::SinkSpec>(spec))
            .added();
    }

    bool LogClient::remove_sink(const SinkID &id)
    {
        return this->call_check(
                       &Stub::remove_sink,
                       ::protobuf::encoded<cc::multilogger::SinkID>(id))
            .removed();
    }

    SinkSpec LogClient::get_sink_spec(const SinkID &id) const
    {
        return protobuf::decoded<SinkSpec>(
            this->call_check(
                &Stub::get_sink,
                ::protobuf::encoded<cc::multilogger::SinkID>(id)));
    }

    SinkSpecs LogClient::get_all_sink_specs() const
    {
        return protobuf::decoded<SinkSpecs>(
            this->call_check(&Stub::get_all_sinks));
    }

    SinkIDs LogClient::list_sinks() const
    {
        return protobuf::assign_to_vector<SinkID>(
            this->call_check(&Stub::list_sinks).sink_names());
    }

    SinkTypes LogClient::list_sink_types() const
    {
        return protobuf::assign_to_vector<SinkType>(
            this->call_check(&Stub::list_sink_types).sink_types());
    }

    FieldNames LogClient::list_static_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::list_static_fields).field_names());
    }

    std::shared_ptr<EventSource> LogClient::listen(const ListenerSpec &spec)
    {
        return ClientListener::create_shared(this->stub, spec);
    }

    void LogClient::open()
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

    void LogClient::close()
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

    void LogClient::capture_event(const core::status::Event::ptr &event)
    {
        if (!this->writer->Write(protobuf::encoded<cc::status::Event>(event)))
        {
            this->close();
        }
    }

}  // namespace multilogger::grpc
