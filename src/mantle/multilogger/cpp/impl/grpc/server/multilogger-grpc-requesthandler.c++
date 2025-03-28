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

    ::grpc::Status RequestHandler::submit(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::Loggable* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->submit(protobuf::decode_loggable(
                *request,
                context->peer()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<::cc::platform::multilogger::Loggable>* reader,
        ::google::protobuf::Empty* response)
    {
        try
        {
            ::cc::platform::multilogger::Loggable loggable;
            while (reader->Read(&loggable))
            {
                this->provider->submit(protobuf::decode_loggable(
                    loggable,
                    context->peer()));
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), "writing to log");
        }
    }

    ::grpc::Status RequestHandler::listen(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::ListenerSpec* request,
        ::grpc::ServerWriter<::cc::platform::multilogger::Loggable>* writer)
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

                writer->Write(protobuf::encoded_shared<cc::platform::multilogger::Loggable>(loggable.value()));
            }

            listener->close();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::add_sink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::SinkSpec* request,
        ::cc::platform::multilogger::AddSinkResult* response)
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

    ::grpc::Status RequestHandler::remove_sink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::SinkID* request,
        ::cc::platform::multilogger::RemoveSinkResult* response)
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

    ::grpc::Status RequestHandler::get_sink(
        ::grpc::ServerContext* context,
        const ::cc::platform::multilogger::SinkID* request,
        ::cc::platform::multilogger::SinkSpec* response)
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

    ::grpc::Status RequestHandler::get_all_sinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::SinkSpecs* response)
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

    ::grpc::Status RequestHandler::list_sinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::SinkNames* response)
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

    ::grpc::Status RequestHandler::list_sink_types(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::SinkTypes* response)
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

    ::grpc::Status RequestHandler::list_message_fields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::FieldNames* response)
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

    ::grpc::Status RequestHandler::list_error_fields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::multilogger::FieldNames* response)
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
