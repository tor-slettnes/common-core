// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-requesthandler.c++
/// @brief Handle MultiLogger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-requesthandler.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "protobuf-message.h++"

namespace multilogger::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::Submit(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::protobuf::Loggable* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->submit(protobuf::decode_loggable(
                *request,
                core::str::url_decoded(context->peer())));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::Writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<::cc::platform::multilogger::protobuf::Loggable>* reader,
        ::google::protobuf::Empty* response)
    {
        try
        {
            ::cc::platform::multilogger::protobuf::Loggable loggable;
            while (reader->Read(&loggable))
            {
                this->provider->submit(protobuf::decode_loggable(
                    loggable,
                    core::str::url_decoded(context->peer())));
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), "writing to log");
        }
    }

    ::grpc::Status RequestHandler::Listen(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::protobuf::ListenerSpec* request,
        ::grpc::ServerWriter<::cc::platform::multilogger::protobuf::Loggable>* writer)
    {
        try
        {
            auto spec = protobuf::decoded<multilogger::ListenerSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(context->peer());
            }

            auto listener = this->provider->listen(spec);
            while (std::optional<core::types::Loggable::ptr> loggable = listener->get())
            {
                if (context->IsCancelled())
                {
                    break;
                }

                writer->Write(protobuf::encoded_shared<cc::platform::multilogger::protobuf::Loggable>(loggable.value()));
            }

            listener->close();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::AddSink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::protobuf::SinkSpec* request,
        ::cc::platform::multilogger::protobuf::AddSinkResult* response)
    {
        try
        {
            auto spec = protobuf::decoded<multilogger::SinkSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(context->peer());
            }

            response->set_added(
                this->provider->add_sink(spec));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::RemoveSink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::protobuf::SinkID* request,
        ::cc::platform::multilogger::protobuf::RemoveSinkResult* response)
    {
        try
        {
            response->set_removed(
                this->provider->remove_sink(request->sink_id()));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetSink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::protobuf::SinkID* request,
        ::cc::platform::multilogger::protobuf::SinkSpec* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_sink_spec(request->sink_id()),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetAllSinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::protobuf::SinkSpecs* response)
    {
        try
        {
            protobuf::encode(
                this->provider->get_all_sink_specs(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ListSinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::protobuf::SinkNames* response)
    {
        try
        {
            protobuf::assign_repeated(
                this->provider->list_sinks(),
                response->mutable_sink_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ListSinkTypes(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::protobuf::SinkTypes* response)
    {
        try
        {
            protobuf::assign_repeated(
                this->provider->list_sink_types(),
                response->mutable_sink_types());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ListMessageFields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::protobuf::FieldNames* response)
    {
        try
        {
            protobuf::assign_repeated(
                this->provider->list_message_fields(),
                response->mutable_field_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ListErrorFields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::protobuf::FieldNames* response)
    {
        try
        {
            protobuf::assign_repeated(
                this->provider->list_error_fields(),
                response->mutable_field_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

}  // namespace multilogger::grpc
