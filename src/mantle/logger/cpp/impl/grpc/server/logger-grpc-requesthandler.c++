// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-requesthandler.c++
/// @brief Handle Logger RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "logger-grpc-requesthandler.h++"
#include "protobuf-logger-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"

namespace logger::grpc
{
    RequestHandler::RequestHandler(const std::shared_ptr<API>& provider)
        : provider(provider)
    {
    }

    ::grpc::Status RequestHandler::submit(
        ::grpc::ServerContext* context,
        const ::cc::status::Event* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            this->provider->submit(protobuf::decoded<core::status::Event::ptr>(
                *request, context->peer()));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::writer(
        ::grpc::ServerContext* context,
        ::grpc::ServerReader<::cc::status::Event>* reader,
        ::google::protobuf::Empty* response)
    {
        try
        {
            ::cc::status::Event event;
            while (reader->Read(&event))
            {
                this->provider->submit(protobuf::decoded<core::status::Event::ptr>(
                    event, context->peer()));
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
        const ::cc::logger::ListenerSpec* request,
        ::grpc::ServerWriter<::cc::status::Event>* writer)
    {
        try
        {
            auto spec = protobuf::decoded<logger::ListenerSpec>(*request);
            if (spec.sink_id.empty())
            {
                spec.sink_id = core::str::url_decoded(context->peer());
            }

            auto listener = this->provider->listen(spec);
            while (std::optional<core::status::Event::ptr> event = listener->get())
            {
                if (context->IsCancelled())
                {
                    break;
                }

                writer->Write(protobuf::encoded<cc::status::Event>(event.value()));
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
        const ::cc::logger::SinkSpec* request,
        ::cc::logger::AddSinkResult* response)
    {
        try
        {
            auto spec = protobuf::decoded<logger::SinkSpec>(*request);
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
        const ::cc::logger::SinkID* request,
        ::cc::logger::RemoveSinkResult* response)
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
        const ::cc::logger::SinkID* request,
        ::cc::logger::SinkSpec* response)
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

    ::grpc::Status RequestHandler::list_sinks(
        ::grpc::ServerContext* context,
        const ::cc::logger::ListSinkRequest* request,
        ::cc::logger::SinkSpecs* response)
    {
        try
        {
            if (request->verbose())
            {
                protobuf::encode(
                    this->provider->get_all_sink_specs(),
                    response);
            }
            else
            {
                protobuf::encode(
                    this->provider->list_sinks(),
                    response);
            }

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
        ::cc::logger::SinkTypes* response)
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

    ::grpc::Status RequestHandler::list_static_fields(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::logger::FieldNames* response)
    {
        try
        {
            protobuf::assign_repeated(
                this->provider->list_static_fields(),
                response->mutable_field_names());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

}  // namespace logger::grpc
