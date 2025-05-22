// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-client.c++
/// @brief Log gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-client.h++"
#include "multilogger-grpc-clientlistener.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-inline.h++"

namespace multilogger::grpc
{
    void LogClient::submit(const core::types::Loggable::ptr &item)
    {
        this->call_check(
            &Stub::submit,
            ::protobuf::encoded_shared<cc::platform::multilogger::Loggable>(item));
    }

    bool LogClient::add_sink(const SinkSpec &spec)
    {
        return this->call_check(
                       &Stub::add_sink,
                       ::protobuf::encoded<cc::platform::multilogger::SinkSpec>(spec))
            .added();
    }

    bool LogClient::remove_sink(const SinkID &id)
    {
        return this->call_check(
                       &Stub::remove_sink,
                       ::protobuf::encoded<cc::platform::multilogger::SinkID>(id))
            .removed();
    }

    SinkSpec LogClient::get_sink_spec(const SinkID &id) const
    {
        return protobuf::decoded<SinkSpec>(
            this->call_check(
                &Stub::get_sink,
                ::protobuf::encoded<cc::platform::multilogger::SinkID>(id)));
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

    FieldNames LogClient::list_message_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::list_message_fields).field_names());
    }

    FieldNames LogClient::list_error_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::list_error_fields).field_names());
    }

    std::shared_ptr<LogSource> LogClient::listen(const ListenerSpec &spec)
    {
        return ClientListener::create_shared(this->stub, spec);
    }

    bool LogClient::is_writer_open() const
    {
        return bool(this->writer);
    }

    void LogClient::open_writer()
    {
        if (!this->writer)
        {
            this->writer_context = std::make_unique<::grpc::ClientContext>();
            this->writer_response = std::make_unique<::google::protobuf::Empty>();
            this->writer = this->stub->writer(this->writer_context.get(),
                                              this->writer_response.get());
        }
    }

    void LogClient::close_writer()
    {
        if (this->writer)
        {
            this->writer->WritesDone();
            this->writer_status = this->writer->Finish();
            this->writer.reset();
            this->writer_context.reset();
        }
    }

    bool LogClient::write_item(const core::types::Loggable::ptr &item)
    {
        if (this->writer->Write(protobuf::encoded_shared<cc::platform::multilogger::Loggable>(item)))
        {
            return true;
        }
        else
        {
            this->close_writer();
            return false;
        }
    }

}  // namespace multilogger::grpc
