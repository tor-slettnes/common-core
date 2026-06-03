// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-requesthandler.c++
/// @brief Handle MultiLogger RPC requests
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-grpc-requesthandler.h++"
#include "protobuf-multilogger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "protobuf-message.h++"

namespace cc::platform::multilogger::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::Submit(
        ::grpc::ServerContext* context,
        const protobuf::Loggable* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->submit(cc::protobuf::decode_loggable(
                *request,
                core::str::url_decoded(this->peer(context))));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::Writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<protobuf::Loggable>* reader,
        ::google::protobuf::Empty* response)
    {
        try
        {
            protobuf::Loggable loggable;
            while (reader->Read(&loggable))
            {
                this->provider->submit(cc::protobuf::decode_loggable(
                    loggable,
                    core::str::url_decoded(this->peer(context))));
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
        const protobuf::ListenerSpec* request,
        ::grpc::ServerWriter<protobuf::Loggable>* writer)
    {
        try
        {
            auto spec = cc::protobuf::decoded<multilogger::ListenerSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(this->peer(context));
            }

            auto listener = this->provider->listen(spec);
            while (std::optional<core::types::Loggable::ptr> loggable = listener->get())
            {
                if (context->IsCancelled())
                {
                    break;
                }
                writer->Write(
                    cc::protobuf::encoded_shared<protobuf::Loggable>(loggable.value()));
            }

            listener->close();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::AddSink(
        ::grpc::ServerContext* context,
        const protobuf::SinkSpec* request,
        protobuf::AddSinkResult* response)
    {
        try
        {
            auto spec = cc::protobuf::decoded<multilogger::SinkSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(this->peer(context));
            }

            response->set_added(
                this->provider->add_sink(spec));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::RemoveSink(
        ::grpc::ServerContext* context,
        const protobuf::SinkID* request,
        protobuf::RemoveSinkResult* response)
    {
        try
        {
            response->set_removed(
                this->provider->remove_sink(request->sink_id()));

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetSink(
        ::grpc::ServerContext* context,
        const protobuf::SinkID* request,
        protobuf::SinkSpec* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_sink_spec(request->sink_id()),
                response);

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetAllSinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        protobuf::SinkSpecs* response)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_all_sink_specs(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListSinks(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        protobuf::SinkNames* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                this->provider->list_sinks(),
                response->mutable_sink_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListSinkTypes(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        protobuf::SinkTypes* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                this->provider->list_sink_types(),
                response->mutable_sink_types());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListMessageFields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        protobuf::FieldNames* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                this->provider->list_message_fields(),
                response->mutable_field_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListErrorFields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        protobuf::FieldNames* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                this->provider->list_error_fields(),
                response->mutable_field_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

}  // namespace cc::platform::multilogger::grpc
