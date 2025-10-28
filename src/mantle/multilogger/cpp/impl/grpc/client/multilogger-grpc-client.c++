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
    void ClientImpl::submit(const core::types::Loggable::ptr &item)
    {
        this->call_check(
            &Stub::Submit,
            ::protobuf::encoded_shared<cc::platform::multilogger::protobuf::Loggable>(item));
    }

    bool ClientImpl::add_sink(const SinkSpec &spec)
    {
        return this->call_check(
                       &Stub::AddSink,
                       ::protobuf::encoded<cc::platform::multilogger::protobuf::SinkSpec>(spec))
            .added();
    }

    bool ClientImpl::remove_sink(const SinkID &id)
    {
        return this->call_check(
                       &Stub::RemoveSink,
                       ::protobuf::encoded<cc::platform::multilogger::protobuf::SinkID>(id))
            .removed();
    }

    SinkSpec ClientImpl::get_sink_spec(const SinkID &id) const
    {
        return protobuf::decoded<SinkSpec>(
            this->call_check(
                &Stub::GetSink,
                ::protobuf::encoded<cc::platform::multilogger::protobuf::SinkID>(id)));
    }

    SinkSpecs ClientImpl::get_all_sink_specs() const
    {
        return protobuf::decoded<SinkSpecs>(
            this->call_check(&Stub::GetAllSinks));
    }

    SinkIDs ClientImpl::list_sinks() const
    {
        return protobuf::assign_to_vector<SinkID>(
            this->call_check(&Stub::ListSinks).sink_names());
    }

    SinkTypes ClientImpl::list_sink_types() const
    {
        return protobuf::assign_to_vector<SinkType>(
            this->call_check(&Stub::ListSinkTypes).sink_types());
    }

    FieldNames ClientImpl::list_message_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::ListMessageFields).field_names());
    }

    FieldNames ClientImpl::list_error_fields() const
    {
        return protobuf::assign_to_vector<std::string>(
            this->call_check(&Stub::ListErrorFields).field_names());
    }

    std::shared_ptr<LogSource> ClientImpl::listen(const ListenerSpec &spec)
    {
        return ClientListener::create_shared(this->stub, spec);
    }

    bool ClientImpl::is_writer_open() const
    {
        return bool(this->writer);
    }

    void ClientImpl::open_writer()
    {
        if (!this->writer)
        {
            this->writer_context = std::make_unique<::grpc::ClientContext>();
            this->writer_context->set_wait_for_ready(true);
            this->writer_response = std::make_unique<::google::protobuf::Empty>();
            this->writer = this->stub->Writer(
                this->writer_context.get(),
                this->writer_response.get());
        }
    }

    void ClientImpl::close_writer()
    {
        if (this->writer)
        {
            this->writer->WritesDone();
            this->writer_status = this->writer->Finish();
            this->writer.reset();
            this->writer_context.reset();
        }
    }

    bool ClientImpl::write_item(const core::types::Loggable::ptr &item)
    {
        if (!this->writer)
        {
            this->open_writer();
        }

        if (this->writer->Write(protobuf::encoded_shared<cc::platform::multilogger::protobuf::Loggable>(item)))
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
